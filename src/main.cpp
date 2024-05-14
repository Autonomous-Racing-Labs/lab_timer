/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/
#include <Arduino.h>
#include <lap_display.h>

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
  Serial.begin(9600);
  lap_display_begin();
  lap_display_reset_timer();

}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
  lap_display_start_timer();
  delay(10000);
  lap_display_lap();
  delay(20000);
  lap_display_reset_timer();
  delay(2000);
}