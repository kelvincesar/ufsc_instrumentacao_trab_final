// Inclusion guard, to prevent multiple includes of the same header
#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

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

#define TEST_SIN_WAVE_SIZE (255)
#define GPIO_LED    GPIO_NUM_16


// Task handles
TaskHandle_t handler_feed_buffer = NULL;
TaskHandle_t handler_uart_send = NULL;

static QueueHandle_t uart0_queue;


// Circular buffer
circular_buffer cb;


// End of the inclusion guard
#endif