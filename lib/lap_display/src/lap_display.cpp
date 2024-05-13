#include "lap_display.h"

// Initialize the static variable
lap_display* lap_display::instance = nullptr;

lap_display::lap_display()
{
    dmd = new DMD(DISPLAYS_ACROSS, DISPLAYS_DOWN);
    uint8_t cpuClock = ESP.getCpuFreqMHz();

    // Use 1st timer of 4; DisplayScan
    // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
    scan_timer = timerBegin(0, cpuClock, true);
    // Attach triggerScan function to our scan_timer
    timerAttachInterrupt(scan_timer, &lap_display::triggerScanStatic, true);
    // Set alarm to call triggerScan function
    // Repeat the alarm (third parameter)
    timerAlarmWrite(scan_timer, 300, true);
    // Start an alarm
    timerAlarmEnable(scan_timer);

    // timer for lap time
    timer = timerBegin(1, cpuClock, true);
    // Attach update display function to timer
    timerAttachInterrupt(timer, &lap_display::update_time_static, true);
    timerAlarmWrite(timer, 1000, true);
    timerAlarmEnable(timer);

    // clear/init the DMD pixels held in RAM
    dmd->clearScreen(true); // true is normal (all pixels off), false is negative (all pixels on)
    dmd->selectFont(Arial_Black_16);

    instance = this;
}

void lap_display::reset_timer()
{
    // Reset the displayed time
    dmd->drawString(0, 0, "00:00:00", 8, GRAPHICS_NORMAL);
    // Stop the timer
    timerAlarmDisable(timer);
}

void lap_display::start_timer()
{
    // Start the timer
    start_time_ms = millis();
    timerAlarmEnable(timer);
}

void lap_display::lap()
{
    show_lap_time_start_ms = millis();
}

void IRAM_ATTR lap_display::triggerScanStatic()
{    
    // Use of static function to call non-static member function
    // Get the instance of lap_display to call triggerScan
    instance->triggerScan();
}

void lap_display::triggerScan()
{
    dmd->scanDisplayBySPI();
}


void IRAM_ATTR lap_display::update_time_static()
{    
    // Use of static function to call non-static member function
    // Get the instance of lap_display to call triggerScan
    instance->update_time();
}

void lap_display::update_time()
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
    char timeStr[9]; // Format: "mm:ss:msms"
    sprintf(timeStr, "%02lu:%02lu:%02lu", minutes, seconds, milliseconds / 10);
    // Display the time
    dmd->clearScreen(true);
    dmd->drawString(0, 0, timeStr, 8, GRAPHICS_NORMAL);

}