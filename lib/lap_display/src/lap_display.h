/*
PINOUT:
DMD | ESP32
OE  - 22
A   - 19
B   - 21
CLK - 18
LAT - 2
DR  - 23

This module shows the lap times on the dmd. It implements the necessary functions to control the timer
*/

#ifndef LAPDISP_H_
#define LAPDISP_H_

#include <Arduino.h>
#include <DMD32.h>
#include "fonts/Arial_Black_16.h"


// Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 1

#define MIN_X_START 0
#define SEC_X_START 18
#define MSEC_X_START 40

#define Y_OFFSET 1

#define SHOW_LAP_TIME_FOR_MS 1000

void lap_display_begin();
void lap_display_reset_timer();
void lap_display_start_timer();
void lap_display_lap();

void IRAM_ATTR triggerScan();

void IRAM_ATTR update_time();


// Timer setup
// create a hardware timer  of ESP32
extern hw_timer_t *update_timer;
extern hw_timer_t *scan_timer;


extern unsigned long start_time_ms;
extern unsigned long show_lap_time_start_ms;


#endif