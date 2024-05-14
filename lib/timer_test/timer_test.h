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

#ifndef TIMERTEST_H_
#define TIMERTEST_H_

#include <Arduino.h>



class timer_test{
    public:
        timer_test();
        void IRAM_ATTR triggerScan();
    
    private:
        
        static void IRAM_ATTR triggerScanStatic();
        static timer_test* instance; // Static variable to hold an instance of lap_display

        hw_timer_t *scan_timer = NULL;

};

#endif