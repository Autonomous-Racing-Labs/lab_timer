
#ifndef STARTLIGHT_H_
#define STARTLIGHT_H_

#include <Arduino.h>
#include "Freenove_WS2812_Lib_for_ESP32.h"


#define LED_BUILTIN 13
#define NUM_LEDS 7
#define DATA_PIN 13
#define CHANNEL 0

#define MODULE_COUNT 5
#define PIXEL_PER_MODULE 7

#define PIXEL_COUNT MODULE_COUNT*PIXEL_PER_MODULE     // make sure to set this to the number of pixels in your strip

#define NEXT_LIGHT_INTERVAL 1000


extern unsigned long StartTime;
extern unsigned long RndmStartTime;


void init_start_lights();
bool play_start_sequence();
void trigger_start();

#endif