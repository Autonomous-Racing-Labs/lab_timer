#include "lap_display.h"


DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Timer setup
// create a hardware timer  of ESP32
hw_timer_t *update_timer = NULL;
hw_timer_t *scan_timer = NULL;


unsigned long start_time_ms = 0;
unsigned long show_lap_time_start_ms = 0;

void lap_display_begin()
{
    uint8_t cpuClock = ESP.getCpuFreqMHz();

    // Use 1st timer of 4; DisplayScan
    // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
    scan_timer = timerBegin(0, cpuClock, true);
    // Attach triggerScan function to our scan_timer
    timerAttachInterrupt(scan_timer, &triggerScan, true);
    // Set alarm to call triggerScan function
    // Repeat the alarm (third parameter)
    timerAlarmWrite(scan_timer, 300, true);
    // Start an alarm
    timerAlarmEnable(scan_timer);

    // timer for lap time
    update_timer = timerBegin(1, cpuClock, true);
    // Attach update display function to timer
    timerAttachInterrupt(update_timer, &update_time, true);
    timerAlarmWrite(update_timer, 1000, true);
    timerAlarmEnable(update_timer);

    // clear/init the DMD pixels held in RAM
    dmd.clearScreen(true); // true is normal (all pixels off), false is negative (all pixels on)
    dmd.selectFont(Arial_Black_16);

}

void lap_display_reset_timer()
{
    // Reset the displayed time
    dmd.drawString(MIN_X_START, Y_OFFSET, "00", 2, GRAPHICS_NORMAL);
    dmd.drawString(SEC_X_START, Y_OFFSET, ":00", 3, GRAPHICS_NORMAL);
    dmd.drawString(MSEC_X_START, Y_OFFSET, ":00", 3, GRAPHICS_NORMAL);
    // Stop the timer
    timerAlarmDisable(update_timer);
}

void lap_display_start_timer()
{
    // Start the timer
    start_time_ms = millis();
    timerAlarmEnable(update_timer);
}

void lap_display_lap()
{
    show_lap_time_start_ms = millis();
}

void triggerScan()
{
    dmd.scanDisplayBySPI();
}



void update_time()
{
    if(millis()-show_lap_time_start_ms < SHOW_LAP_TIME_FOR_MS){
        // stop the display time updates for SHOW_LAP_TIME_FOR_MS
        return;
    }
    // Get the current time
    unsigned long currentTime = millis()-start_time_ms;
    unsigned long milliseconds = currentTime % 1000;
    unsigned long seconds = (currentTime / 1000) % 60;
    unsigned long minutes = (currentTime / 60000) % 60;
    // Convert time to string format
    char timeStrMin[3]; // Format: "mm:ss:msms"
    sprintf(timeStrMin, "%02lu", minutes);
    char timeStrSec[4]; // Format: "mm:ss:msms"
    sprintf(timeStrSec, ":%02lu", seconds);
    char timeStrMsec[4]; // Format: "mm:ss:msms"
    sprintf(timeStrMsec, ":%02lu", milliseconds / 10);
    // Display the time
    dmd.clearScreen(true);
    dmd.drawString(MIN_X_START, Y_OFFSET, timeStrMin, 2, GRAPHICS_NORMAL);
    dmd.drawString(SEC_X_START, Y_OFFSET, timeStrSec, 3, GRAPHICS_NORMAL);
    dmd.drawString(MSEC_X_START, Y_OFFSET, timeStrMsec, 3, GRAPHICS_NORMAL);

}