// #include <Arduino.h>
// #include "Freenove_WS2812_Lib_for_ESP32.h"

// #define LEDS_COUNT  35
// #define LEDS_PIN	13
// #define CHANNEL		0

// Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);


// void setup() {
// 	strip.begin();
// 	strip.setBrightness(255);  
  
//   strip.set_pixel(1, 255,255,0);
//   strip.show();
//   delay(1000);
//   strip.set_pixel(2, 255,0,0);
//   strip.show();
//   delay(1000);
//   strip.set_pixel(3, 0,255,0);
//   strip.show();

//   delay(5000);
//   strip.setAllLedsColorData(0);
//   strip.show();
//   // for (int i = 0; i < LEDS_COUNT; i++) {
//   //   strip.setLedColorData(i, 255, 0, 0);
//   //   strip.show();
//   //   delay(delayval);
//   // }
// }
// void loop() {


// }

#include <Arduino.h>
#include "start_lights.h"


void setup()
{
  init_start_lights();

  delay(1000);
  trigger_start();
}

void loop()
{
  play_start_sequence();
}

