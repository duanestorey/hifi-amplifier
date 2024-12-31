#include <stdio.h>
#include <driver/i2c.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_wifi.h" 
#include "esp_event.h" 
#include "esp_log.h"
#include "nvs_flash.h"

// Our amplifier files
#include "amplifier.h"
#include "wifi.h"

// Main Display Thread
void 
startDisplayThread( void *param ) {
    // Proxy thread start to main amplifier thread
    ((Amplifier *)param)->handleDisplayThread();
}

// Main Amplifier Thread
void 
startAudioThread( void *param ) {
    // Proxy thread start to main amplifier thread
    ((Amplifier *)param)->handleAudioThread();
}

void
startRadioThread( void *param ) {
    // Proxy thread start to main radio thread
    ((Amplifier *)param)->handleRadioThread();
}

void
startTimerThread( void *param ) {
    // Proxy thread start to main radio thread
    ((Amplifier *)param)->handleTimerThread();
}

void
startAmplifierThread( void *param ) {
    // Proxy thread start to main radio thread
    ((Amplifier *)param)->handleAmplifierThread();
}

extern "C" void app_main(void)
{
    Amplifier *amp = new Amplifier;
    amp->init();

    xTaskCreate(
        startTimerThread,
        "Timer Thread",
        10000,
        (void *)amp,
        2,
        NULL
    );   

    xTaskCreate(
        startDisplayThread,
        "Display Thread",
        10000,
        (void *)amp,
        2,
        NULL
    );

    xTaskCreate(
        startAudioThread,
        "Audio Thread",
        10000,
        (void *)amp,
        2,
        NULL
    );

    xTaskCreate(
        startRadioThread,
        "Radio Thread",
        10000,
        (void *)amp,
        2,
        NULL
    ); 

    xTaskCreate(
        startAmplifierThread,
        "Amplifier Thread",
        10000,
        (void *)amp,
        3,
        NULL
    );   

    while ( true ) {
        vTaskDelay( 1000 / portTICK_PERIOD_MS );
    }
}
