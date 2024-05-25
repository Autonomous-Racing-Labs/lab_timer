#include <Arduino.h>
#include "start_lights.h"
#include "lap_display.h"

enum State {
  INIT,
  PLAY_START_SEQUENCE,
  RACE,
  STOP_RACE
};

State currentState = INIT;

const int startBtnPin = 32;  // Pin connected to start button
bool startBtnPressed = false;

const int lightBarrierPin = 22;
bool lightBarrierTriggered = false;

// Function declarations
void initSoftwareModules();
bool playStartSequence();
void race();
void stopRace();
void checkButtonPress();

void setup()
{
  Serial.begin(9600);
  pinMode(startBtnPin, INPUT);
  pinMode(lightBarrierPin, INPUT);
  initSoftwareModules();
  delay(1000);
  Serial.println("start machine");
}

void loop()
{
  checkButtonPress();
  
  switch (currentState) {
    case INIT:
      if (startBtnPressed) {
        Serial.println("start_triggered");
        currentState = PLAY_START_SEQUENCE;
        startBtnPressed = false;
        trigger_start();
      }
      break;

    case PLAY_START_SEQUENCE:
      if (play_start_sequence()) {
        Serial.println("start sequence_end");
        currentState = RACE;
        lap_display_start_timer();
      }
      break;

    case RACE:
      race();
      if (startBtnPressed) {
        Serial.println("end_race");
        currentState = STOP_RACE;
        startBtnPressed = false;
        lap_display_reset_timer();
      }
      break;

    case STOP_RACE:
      stopRace();
      if (startBtnPressed) {
        Serial.println("goto play start sequence");
        currentState = PLAY_START_SEQUENCE;
        startBtnPressed = false;
        trigger_start();
      }
      break;
  }
}

void initSoftwareModules() {
  // Initialize software modules
  init_start_lights();
  lap_display_begin();
  lap_display_reset_timer();
}

void race() {
  // Race logic
  // Pseudo code: Measure lap times, display current lap, etc.
  if(digitalRead(lightBarrierPin) == HIGH && lightBarrierTriggered == false){
    lap_display_lap();
    lightBarrierTriggered = true;
  }else{
    lightBarrierTriggered = false;
  }
}

void stopRace() {
  // Stop race logic
  // Pseudo code: Finalize results, display summary, etc.
}

void checkButtonPress() {
  if (digitalRead(startBtnPin) == HIGH) {
    delay(50); // Debounce delay
    if (digitalRead(startBtnPin) == HIGH) {
      Serial.println("start btn press");
      startBtnPressed = true;
      while (digitalRead(startBtnPin) == HIGH); // Wait for button release
      Serial.println("Relased btnf");
    }
  }
}

