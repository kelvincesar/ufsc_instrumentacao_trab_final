// Inclusion guard, to prevent multiple includes of the same header
#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "sdkconfig.h"

#include <driver/gpio.h>                    // ESP-IDF library for gpio control
#include <driver/i2s.h>                     // ESP-IDF library for i2s drive
#include <driver/adc.h>                     // ESP-IDF library for adc drive
#include <esp_adc_cal.h>                    // ESP-IDF library for ADC calibration

#include "esp_err.h"						// ESP-IDF library for error codes and error handling
#include "esp_log.h"						// ESP-IDF logging library
#include "esp_system.h"
// - Watchdog
#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"

// Application lib files
#include "circular_buffer.h"     // Circular Buffer


// Application defines

#define UART_PORT UART_NUM_0
#define UART_BUFF_SIZE (2048)
#define UART_MEASURES_SIZE (85)


#define GPIO_LED    GPIO_NUM_16
#define CIRCULAR_BUFFER_SIZE    (25500)     // Quantidade de amostras que o buffer circular armazena.
#define ADC_I2S_NUM             (0)         // I2S drive number;
#define ADC_SAMPLE_RATE         (6000)      // Sampling rate in Hz
#define ADC_DMA_COUNT           (24)        // Number of DMA buffers
#define ADC_BUFFER_SIZE         (100)       // I2S Buffer size (limit: 1024)
#define ADC_VOLTAGE_CHANNEL     (5)         // ADC channel used
#define ADC_GET_MEASURE(s)      (s & 0xFFF) // Macro used to get 12 bit part from adc read;




// Task handles
TaskHandle_t handler_feed_buffer = NULL;
TaskHandle_t handler_uart_send = NULL;
TaskHandle_t handler_sample_task = NULL;

static QueueHandle_t uart0_queue;
static const char *TAG = "Main";	        // Define general log tag

// Circular buffer
circular_buffer cb;


// End of the inclusion guard
#endif