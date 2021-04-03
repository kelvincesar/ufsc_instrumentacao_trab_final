// Include used libraries
#include "main.h"					// Smartmeter header




void setup_usb_uart(){
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };

    //Install UART driver, and get the queue.
    uart_driver_install(UART_PORT, UART_BUFF_SIZE * 2, UART_BUFF_SIZE * 2, 20, &uart0_queue, 0);
    uart_param_config(UART_PORT, &uart_config);
    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(UART_PORT, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

}


static void uart_send_data(void *pvParameters)
{
    uart_event_t event;
    uint16_t data_len;
    int handler_cb;
    
    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(sizeof(uint8_t)*UART_BUFF_SIZE);   // Alocação de dados 
    char *sent_array = (char *) malloc(sizeof(char)*UART_MEASURES_SIZE*3);   // vetor para envio de dados 
    // headers
    
    uint16_t measure = 0;
    uint8_t index = 0;
    uint8_t flag_send = 0;
    uint8_t array_size = 0;
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(data, UART_BUFF_SIZE);
            flag_send = 0;
            array_size = 0;

            // Received data on UART
            if(event.type == UART_DATA) {
                // Read data from the UART
                //printf("Reading uart \n");
                data_len = uart_read_bytes(UART_PORT, data, event.size, portMAX_DELAY);
                if(data_len > 0){
                    //printf("Received data \n");
                    //printf("received: %s", (const char*) data);
                    for (index = 0; index < UART_MEASURES_SIZE*3; index+=3){
                        handler_cb = cb_pop(&cb, &measure);
                        //printf("Buffer read \n");
                        if(handler_cb == 0){ 
                            //printf("data: %d\n",  measure);
                            // Write data back to the UART
                            sent_array[index] = 0;    // measure header
                            sent_array[index+1] = measure >> 8;
                            sent_array[index+2] = measure & 0xFF;
                            flag_send = 1;
                            array_size += 3;
                        } else break;                        
                    }
                    if(flag_send) uart_write_bytes(UART_PORT, (const char*)sent_array, array_size);
                    
                }
            }
        }
    }
    cb_clean(&cb);
    free(data);
    data = NULL;
    vTaskDelete(NULL);
}
void feed_buffer(){
    static uint16_t table_sin_wave[TEST_SIN_WAVE_SIZE] = {
    // Sin wave
        128,131,134,137,141,144,147,150,153,156,159,162,165,168,171,174,
        177,180,183,186,188,191,194,196,199,202,204,207,209,212,214,216,218,221,223,225,227,229,231,233,234,236,
        238,239,241,242,243,245,246,247,248,249,250,251,252,253,253,254,254,255,255,255,255,255,255,
        255,255,255,255,254,254,253,253,252,251,251,250,249,248,247,245,244,243,241,240,238,
        237,235,233,232,230,228,226,224,222,220,217,215,213,210,208,205,203,200,198,195,192,190,187,184,181,178,176,
        173,170,167,164,161,158,155,151,148,145,142,139,136,133,130,126,123,120,117,114,111,108,105,101,
        98,95,92,89,86,83,80,78,75,72,69,66,64,61,58,56,53,51,48,46,43,41,39,36,34,32,30,28,26,24,23,21,
        19,18,16,15,13,12,11,9,8,7,6,5,5,4,3,3,2,2,1,1,1,1,1,1,1,1,1,1,2,2,3,3,4,5,6,7,8,9,10,
        11,13,14,15,17,18,20,22,23,25,27,29,31,33,35,38,40,42,44,47,49,52,54,
        57,60,62,65,68,70,73,76,79,82,85,88,91,94,97,100,103,106,109,112,115,119,122,125
    };

    uint16_t i = 0;
    int handler_cb;
    int64_t start_time = 0;
    while (true){

        // To not hog the CPU
        if( (esp_timer_get_time() - start_time >= 60) || start_time == 0){   // Check delta in uS
            handler_cb = cb_push(&cb, &table_sin_wave[i]);
            if(handler_cb > 0) gpio_set_level(GPIO_LED, 1);
            else gpio_set_level(GPIO_LED, 0);
            i++;
            if (i >= TEST_SIN_WAVE_SIZE) i = 0;

            start_time = esp_timer_get_time();
        }
        // Feed watchdog
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;

    }

}
void app_main(void)
{
    BaseType_t task_create_ret;				// Return of task create
    int handler_cb;

    // Setup UART to USB;
    setup_usb_uart();
    // Setup GPIO 
    gpio_pad_select_gpio(GPIO_LED);
    gpio_set_direction(GPIO_LED, GPIO_MODE_DEF_OUTPUT);

    // Setup circular buffer
    handler_cb = cb_init(&cb, 25500, sizeof(uint16_t));

	if(handler_cb == CB_SUCCESS){
        //printf("* Buffer initialized.\n");

        // Create task to generate a signal
        task_create_ret = xTaskCreatePinnedToCore(
            feed_buffer,			// Function executed in the task
            "FB",					// Task name (for debug)
            2046,					// Stack size in bytes
            NULL,					// Parameter to pass to the function
            1,						// Task priority
            &handler_feed_buffer,	// Used to pass back a handle by which the created task can be referenced
            1                       // CPU core ID
        );						

        // Check task creation error
        if (task_create_ret != pdPASS) ESP_LOGE("MAIN", "Error creating feed_buffer task");

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

    }
}