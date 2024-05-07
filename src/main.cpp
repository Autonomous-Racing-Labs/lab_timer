#include <Arduino.h>
#include <FastLED.h>

#define LED_BUILTIN 12
#define NUM_LEDS 7
#define DATA_PIN 15

CRGB leds[NUM_LEDS];

void setup() {
  // put your setup code here, to run once:
  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

void loop() {
  for (CRGB led : leds) {
    led = CRGB::Red;
  }
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  for (CRGB led : leds) {
    led = CRGB::Black;
  }
  FastLED.show();
  delay(500);

}
