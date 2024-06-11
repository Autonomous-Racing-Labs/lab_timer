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

#define SEC_X_START 1
#define MSEC_X_START 19

#define DISPLAY_WIDTH 32
#define DISPLAY_HEIGHT 16

#define Y_OFFSET 1

#define SHOW_LAP_TIME_FOR_MS 1000

void lap_display_begin();
void lap_display_reset_timer();
void reset_displayed_time_A();
void reset_displayed_time_B();
void lap_display_start_timer_A();
void lap_display_start_timer_B();
void lap_display_lap_A();
void lap_display_lap_B();
void disable_timer_interrupts();
void enable_timer_interrupts();
void draw_time_A(unsigned long display_time_ms);
void draw_time_B(unsigned long display_time_ms);

void lap_display_start_new_lap_A();
void lap_display_start_new_lap_B();
void display_best_times();

void IRAM_ATTR triggerScan();

void IRAM_ATTR update_time();
void IRAM_ATTR update_time_A();
void IRAM_ATTR update_time_B();


// Timer setup
// create a hardware timer  of ESP32
extern hw_timer_t *update_timer;
extern hw_timer_t *scan_timer;


extern unsigned long start_time_ms_A;
extern unsigned long start_time_ms_B;
extern unsigned long show_lap_time_start_ms_A;
extern unsigned long show_lap_time_start_ms_B;

extern unsigned long display_time_A;
extern unsigned long display_time_B;

extern unsigned int min_lap_time_A;
extern unsigned int min_lap_time_B;

#endif