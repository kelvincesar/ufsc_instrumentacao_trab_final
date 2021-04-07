/* ********************************************************************************
 * UNIVERSIDADE FEDERAL DE SANTA CATARINA - JOINVILLE
 * ESE410009-41010086ME (20203) - Instrumentação para Sistemas Eletrônicos
 * 
 * Aluno: Kelvin César de Andrade
 * Data: 04/2021
 * Descrição: 
 * 
 * - Trabalho final para amostrar um sinal e transmitir para o software LabVIEW 
 * via UART;
 * 
 * - Neste projeto é realizado a amostragem de um sinal do sensor de tensão AC 
 * ZMPT101B via I2S e armazenado em um buffer circular. Uma outra task é criada
 *  no núcleo 1 para realizar a transmissão via UART das amostras coletadas.
 * 
 * - Core 0: Amostragem do sinal via I2S.
 * - Core 1: Transmissão dos dados via UART.
 *********************************************************************************
 */

// Include header with libraries and defines
#include "main.h"




void setup_usb_uart(){
    // Configure parameters of an UART driver
    uart_config_t uart_config = {
        .baud_rate = 460800,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    //Install UART driver
    uart_driver_install(UART_PORT, UART_BUFF_SIZE * 2, UART_BUFF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(UART_PORT, &uart_config);
    //Set UART pins to UART 0
    uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

}


static void uart_send_data(void *pvParameters)
{
    uart_event_t event;
    uint16_t data_len;
    int handler_cb;
    
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(sizeof(uint8_t)*UART_BUFF_SIZE);     // Alocação de dados 
    char *sent_array = (char *) malloc(sizeof(char)*UART_MEASURES_SIZE*3);  // Vetor para envio de dados 
    // headers
    
    uint16_t measure = 0;
    uint8_t flag_send = 0;
    uint8_t array_size = 0;

    // Loop forever waiting for UART requests
    for(;;) {

        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            // Clean uart buffer
            bzero(data, UART_BUFF_SIZE);

            // Checks uart event (data received)
            if(event.type == UART_DATA) {
                flag_send = 0;
                array_size = 0;
                measure  = 0;

                // Read data from UART
                data_len = uart_read_bytes(UART_PORT, data, event.size, portMAX_DELAY);
                if(data_len > 0){
                    // Loop on to colect UART_MEASURES_SIZE to send to LabVIEW
                    while(array_size < UART_MEASURES_SIZE*3){
                        // Read circular buffer
                        handler_cb = cb_pop(&cb, &measure);
                        // If success...
                        if(handler_cb == 0){ 
 
                            // Write data on UART array
                            sent_array[array_size] = 0;                 // measure header
                            sent_array[array_size+1] = measure >> 8;    // measure high byte
                            sent_array[array_size+2] = measure & 0xFF;  // measure low byte
                            flag_send = 1;
                            array_size += 3;
                        } else {
                            vTaskDelay(pdMS_TO_TICKS(1)); // Delay 1ms waiting for more measures 
                        };                        
                    }

                    // Send data on UART
                    if(flag_send) uart_write_bytes(UART_PORT, (const char*)sent_array, array_size);
                    
                }
            }
        }
    }
    // Clean buffers
    cb_clean(&cb);
    free(data);
    data = NULL;

    // Kill task
    vTaskDelete(NULL);
}

// Inicialização do I2S
static void adc_i2s_init(void)
{
    
    static const i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_ADC_BUILT_IN,
        .sample_rate = ADC_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = ADC_DMA_COUNT,
        .dma_buf_len = ADC_BUFFER_SIZE,
        .use_apll = false,
    };
    //install and start i2s driver
    i2s_driver_install(ADC_I2S_NUM, &i2s_config, 0, NULL);

    // Some delay.
    vTaskDelay(5000/portTICK_RATE_MS);

    i2s_set_adc_mode(ADC_UNIT_1, ADC_VOLTAGE_CHANNEL);

}

void adc_sampling(void *parameters){

    // # Define ADC Characteristics for convertion
    esp_adc_cal_characteristics_t cal;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, 3300, &cal);

    
    // # Declare ADC variables
    uint16_t buf[ADC_BUFFER_SIZE];  // Raw buffer with measures from ADC;  
    size_t measures_read;           // Receive number of measures read from I2S - ADC
    
    // # Declare local variables
    uint16_t sample = 0;            // Generic variable to momently store sample value converted to mV
    uint16_t loop_ctrl = 0;         // Used to control while loop
    int handler_cb; 
    
    // # Start ADC I2S Setup
    adc_i2s_init();


    while (loop_ctrl == 0) {
        // # Read data from i2s.      
        esp_err_t err = i2s_read(ADC_I2S_NUM, buf, sizeof(buf), &measures_read, portMAX_DELAY);

        if (err != ESP_OK)
        {
            printf("i2s_read: %d\n", err);

        }
        // # Compute vector size
        measures_read /= sizeof(uint16_t);  

        // # Loop on measures to convert and save on buffer
        for (uint16_t i = 0; i < measures_read; i ++)
        {
            // Get channel offset on buffer
            switch (buf[i] >> 12){
                case ADC_VOLTAGE_CHANNEL:
                    // Read sample (get 12 first bits)
                    sample = ADC_GET_MEASURE(buf[i]);

                    // Insert on circular buffer
                    handler_cb = cb_push(&cb, &sample);

                    // Indicate on LED (buffer status)
                    if(handler_cb > 0) gpio_set_level(GPIO_LED, 1);
                    else gpio_set_level(GPIO_LED, 0);
              
                    break; 
            } 
        }
    }

    i2s_adc_disable(ADC_I2S_NUM);
    i2s_stop(ADC_I2S_NUM);

    vTaskDelete(NULL);
    
}


void app_main(void){
    BaseType_t task_create_ret;				// Return of task create
    int handler_cb;

    // Setup UART to USB;
    setup_usb_uart();
    // Setup GPIO 
    gpio_pad_select_gpio(GPIO_LED);
    gpio_set_direction(GPIO_LED, GPIO_MODE_DEF_OUTPUT);

    // Setup circular buffer
    handler_cb = cb_init(&cb, CIRCULAR_BUFFER_SIZE, sizeof(uint16_t));

	if(handler_cb == CB_SUCCESS){
        // Create task to send data over UART
        task_create_ret = xTaskCreatePinnedToCore(
            uart_send_data,			// Function executed in the task
            "UART",					// Task name (for debug)
            2046,					// Stack size in bytes
            NULL,					// Parameter to pass to the function
            1,						// Task priority
            &handler_uart_send,	    // Used to pass back a handle by which the created task can be referenced
            0                       // CPU core ID
        );						

        // Check task creation error
        if (task_create_ret != pdPASS) ESP_LOGE("MAIN", "Error creating uart_send_data task");

        // Create main task to sample and process signals
            task_create_ret = xTaskCreatePinnedToCore(
                adc_sampling,					    // Function executed in the task
                "SRT",					            // Task name (for debug)
                32000,								// Stack size in bytes
                NULL,								// Parameter to pass to the function
                1,									// Task priority
                &handler_sample_task,			    // Used to pass back a handle by which the created task can be referenced
                0);									// CPU core ID

            // Check task creation error
            if (task_create_ret != pdPASS){ ESP_LOGE(TAG, "Error creating adc_sampling task"); }
    }
}
