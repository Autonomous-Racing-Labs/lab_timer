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

#define SHOW_LAP_TIME_FOR_MS 3000

class lap_display{
    public:
        lap_display();
        void reset_timer();
        void start_timer();
        void lap();
    
    private:
        void IRAM_ATTR triggerScan();
        static void IRAM_ATTR triggerScanStatic();

        void IRAM_ATTR update_time();
        static void IRAM_ATTR update_time_static();

        DMD * dmd = NULL;
        // Timer setup
        // create a hardware timer  of ESP32
        hw_timer_t *timer = NULL;
        hw_timer_t *scan_timer = NULL;
        static lap_display* instance; // Static variable to hold an instance of lap_display

        unsigned long start_time_ms = 0;
        unsigned long show_lap_time_start_ms = 0;
};

#endif