#include <stdio.h>
#include <stdlib.h>

#define TEST_SIN_WAVE_SIZE (255)
#define GPIO_LED    GPIO_NUM_16

#include "soc/timer_group_reg.h"
#include "soc/timer_group_struct.h"
// Função utilizada para validar a comunicação com o LabVIEW gerando dados
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
        if( (esp_timer_get_time() - start_time >= 167) || start_time == 0){   // Check delta in uS
            handler_cb = cb_push(&cb, &table_sin_wave[i]);

            if(handler_cb > 0) gpio_set_level(GPIO_LED, 1);
            else gpio_set_level(GPIO_LED, 0);

            i++;
            if (i >= TEST_SIN_WAVE_SIZE) i = 0;

            start_time = esp_timer_get_time();
        }
        // Feed the watchdog
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;

    }

}