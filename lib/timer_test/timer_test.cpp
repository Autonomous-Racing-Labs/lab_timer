#include "timer_test.h"

timer_test* timer_test::instance = nullptr;

timer_test::timer_test()
{
    Serial.begin(9600);
    Serial.println("start");
    uint8_t cpuClock = ESP.getCpuFreqMHz();

    // Use 1st timer of 4; DisplayScan
    // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
    scan_timer = timerBegin(0, cpuClock, true);
    // Attach triggerScan function to our scan_timer
    timerAttachInterrupt(scan_timer, &timer_test::triggerScanStatic, true);
    // Set alarm to call triggerScan function
    // Repeat the alarm (third parameter)
    timerAlarmWrite(scan_timer, 500000, true);
    // Start an alarm
    timerAlarmEnable(scan_timer);

    instance = this;
}


void timer_test::triggerScanStatic()
{
    instance->triggerScan();
}

void timer_test::triggerScan()
{
    Serial.println("hello from test");
}

