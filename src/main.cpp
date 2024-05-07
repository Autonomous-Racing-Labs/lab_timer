/*
Pin connections
DMD | ESP
LAT - 22
A   - 19
B   - 23
OE  - 16
R   - 13
CLK - 14
*/

#include <Arduino.h>
#include <PxMatrix.h>

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_OE 16
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// Pins for LED MATRIX

PxMATRIX display(32,16,P_LAT, P_OE,P_A,P_B);
//PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
//PxMATRIX display(64,64,P_LAT, P_OE,P_A,P_B,P_C,P_D,P_E);


void IRAM_ATTR display_updater(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  //isplay.display(70);
  display.displayTestPattern(70);
  portEXIT_CRITICAL_ISR(&timerMux);
}


uint16_t myCYAN = display.color565(255, 255, 255);
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  display.begin(4);
  // display.setScanPattern(ZAGGIZ);
  // display.setMuxPattern(BINARY);
  display.flushDisplay();
  display.setTextColor(myCYAN);
  display.setCursor(0,0);
  display.print("Pixel");
  Serial.println("hello");

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 4000, true);
  timerAlarmEnable(timer);


  delay(1000);
}


void loop() {

 delay(100);

}
