#include "lap_display.h"

DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);

// Timer setup
// create a hardware timer  of ESP32
hw_timer_t *update_timer = NULL;
hw_timer_t *scan_timer = NULL;

bool timerEnabled = false;

unsigned long start_time_ms_A = 0;
unsigned long start_time_ms_B = 0;
unsigned long show_lap_time_start_ms_A = 0;
unsigned long show_lap_time_start_ms_B = 0;

unsigned long display_time_A = 0;
unsigned long display_time_B = 0;

unsigned int min_lap_time_A = 0;
unsigned int min_lap_time_B = 0;

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
    // Stop the timer
    timerAlarmDisable(update_timer);
    // Reset the displayed time
    reset_displayed_time_A();
    reset_displayed_time_B();
}
void reset_displayed_time_A()
{
    dmd.drawString(SEC_X_START, Y_OFFSET, "00", 2, GRAPHICS_NORMAL);
    dmd.drawString(MSEC_X_START, Y_OFFSET, ":0", 2, GRAPHICS_NORMAL);
}
void reset_displayed_time_B()
{
    dmd.drawString(SEC_X_START + DISPLAY_WIDTH, Y_OFFSET, "00", 2, GRAPHICS_NORMAL);
    dmd.drawString(MSEC_X_START + DISPLAY_WIDTH, Y_OFFSET, ":0", 2, GRAPHICS_NORMAL);
}
void lap_display_start_timer_A()
{
    // Start the timer
    start_time_ms_A = millis();
    timerAlarmEnable(update_timer);
}
void lap_display_start_timer_B()
{
    // Start the timer
    start_time_ms_B = millis();
    timerAlarmEnable(update_timer);
}

void lap_display_lap_A()
{
    show_lap_time_start_ms_A = millis();
}
void lap_display_lap_B()
{
    show_lap_time_start_ms_B = millis();
}

void lap_display_start_new_lap_A()
{
    unsigned long lap_time = 0;

    if (start_time_ms_A > 0)
    {
        lap_time = millis() - start_time_ms_A;
        lap_display_lap_A();
    }
    if(min_lap_time_A == 0 || lap_time < min_lap_time_A){
        Serial.print("new record time");
        min_lap_time_A = lap_time;
        Serial.println(min_lap_time_A);
    }
    lap_display_start_timer_A();

}

void lap_display_start_new_lap_B()
{
    unsigned long lap_time = 0;

    if (start_time_ms_B > 0)
    {
        lap_time = millis() - start_time_ms_B;
        lap_display_lap_B();
    }
    if(min_lap_time_B == 0 || lap_time < min_lap_time_B){
        min_lap_time_B = lap_time;
    }
    lap_display_start_timer_B();

}

void display_best_times(){
    timerAlarmDisable(update_timer);

    draw_time_A(min_lap_time_A);
    draw_time_B(min_lap_time_B);
}

void triggerScan()
{
    dmd.scanDisplayBySPI();
}

void update_time()
{
    dmd.clearScreen(true);
    update_time_A();
    update_time_B();
}

void update_time_A()
{
    if (millis() - show_lap_time_start_ms_A < SHOW_LAP_TIME_FOR_MS)
    {
        // stop the display time updates for SHOW_LAP_TIME_FOR_MS
    }
    else if(start_time_ms_A == 0){
        display_time_A = 0;
    }
    else
    {
        display_time_A = millis() - start_time_ms_A;
    }
    draw_time_A(display_time_A);
}
void update_time_B()
{
    if (millis() - show_lap_time_start_ms_B < SHOW_LAP_TIME_FOR_MS)
    {
        // stop the display time updates for SHOW_LAP_TIME_FOR_MS
    }
    else if(start_time_ms_B == 0){
        display_time_B = 0;
    }
    else
    {
        display_time_B = millis() - start_time_ms_B;
    }

    draw_time_B(display_time_B);
}
void draw_time_A(unsigned long currentTime)
{
    int x_offset = 0;
    // if(x_offset > 0 && start_time_ms > 0){
    //     Serial.print("B_Time is now shown: ");
    //     Serial.println(start_time_ms);
    // }
    if(currentTime == 0 && false){
        dmd.drawString(SEC_X_START + x_offset, Y_OFFSET, "00", 2, GRAPHICS_NORMAL);
        dmd.drawString(MSEC_X_START + x_offset, Y_OFFSET, ":0", 2, GRAPHICS_NORMAL);
    
    }else{
        // // Get the current time
        // unsigned long currentTime = millis() - start_time_ms;
        unsigned long milliseconds = (currentTime % 1000) / 10;
        unsigned long seconds = (currentTime / 1000) % 60;
        // Convert time to string format
        char timeStrSec[3]; // Format: "ss:ms"
        sprintf(timeStrSec, "%02lu", seconds);
        char timeStrMsec[3]; // Format: "ss:ms"
        sprintf(timeStrMsec, ":%01lu", milliseconds / 10);
        // Display the time
        dmd.drawString(SEC_X_START + x_offset, Y_OFFSET, timeStrSec, 2, GRAPHICS_NORMAL);
        dmd.drawString(MSEC_X_START + x_offset, Y_OFFSET, timeStrMsec, 2, GRAPHICS_NORMAL);
    }
}

void draw_time_B(unsigned long currentTime)
{
    int x_offset = DISPLAY_WIDTH;
    // if(x_offset > 0 && start_time_ms > 0){
    //     Serial.print("B_Time is now shown: ");
    //     Serial.println(start_time_ms);
    // }
    if(currentTime == 0 && false){
        dmd.drawString(SEC_X_START + x_offset, Y_OFFSET, "00", 2, GRAPHICS_NORMAL);
        dmd.drawString(MSEC_X_START + x_offset, Y_OFFSET, ":0", 2, GRAPHICS_NORMAL);
    
    }else{
        // // Get the current time
        // unsigned long currentTime = millis() - start_time_ms;
        unsigned long milliseconds = (currentTime % 1000) / 10;
        unsigned long seconds = (currentTime / 1000) % 60;
        // Convert time to string format
        char timeStrSec[3]; // Format: "ss:ms"
        sprintf(timeStrSec, "%02lu", seconds);
        char timeStrMsec[3]; // Format: "ss:ms"
        sprintf(timeStrMsec, ":%01lu", milliseconds / 10);
        // Display the time
        dmd.drawString(SEC_X_START + x_offset, Y_OFFSET, timeStrSec, 2, GRAPHICS_NORMAL);
        dmd.drawString(MSEC_X_START + x_offset, Y_OFFSET, timeStrMsec, 2, GRAPHICS_NORMAL);
    }
}
void disable_timer_interrupts()
{
    if (update_timer != NULL and scan_timer != NULL)
    {
        timerDetachInterrupt(update_timer);
        timerDetachInterrupt(scan_timer);
    }
}
void enable_timer_interrupts()
{
    if (update_timer != NULL and scan_timer != NULL)
    {
        // timerAttachInterrupt(update_timer, &update_time, true);
        // timerAttachInterrupt(scan_timer, &triggerScan, true);

        timerAttachInterrupt(scan_timer, &triggerScan, true);
        // Set alarm to call triggerScan function
        // Repeat the alarm (third parameter)
        timerAlarmWrite(scan_timer, 300, true);
        // Start an alarm
        timerAlarmEnable(scan_timer);

        timerAttachInterrupt(update_timer, &update_time, true);
        // Set alarm to call triggerScan function
        // Repeat the alarm (third parameter)
        timerAlarmWrite(update_timer, 300, true);
    }
}