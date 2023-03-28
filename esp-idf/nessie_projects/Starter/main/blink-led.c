#include <stdio.h>

// including free rtos 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// STARTER CODE :D
#include "esp_log.h"



void app_main(void)
{
    // Inbuilt stuff 
    char *ourTaskName = pcTaskGetName(NULL);
    
    // ESP log is a function that writes data out to the serial console on the esp32 (Nessie)
    ESP_LOGI(ourTaskName, "Hello! Nessie is starting up :)!\n");    




    // Just temporary inf loop to prevent a crash.
    while(1) {

        vTaskDelay(1000 );
    }
}
