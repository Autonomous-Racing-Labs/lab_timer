#include <Arduino.h>

#include "start_lights.h"
#include "lap_display.h"
#include "uros_action.h"

enum State {
  INIT,
  AWAIT_START_BTN,
  REQUEST_READY_STATUS,
  PLAY_START_SEQUENCE,
  START_RACE,
  RACE,
  STOP_RACE
};

State currentState = INIT;

RosComm* carA = nullptr;
// RosComm* carB = nullptr;

const int startBtnPin = 32;  // Pin connected to start button
bool startBtnPressed = false;

const int lightBarrierPin = 22;
bool lightBarrierTriggered = false;

unsigned long startRequestReady_ms = 0;
const unsigned long requestReadyTimeout_ms = 10000;

bool is_a_ready = false;
bool is_b_ready = false;
bool is_timeout = false;

// Function declarations
void initSoftwareModules();
bool playStartSequence();
void race();
void stopRace();
void checkButtonPress();

void setup()
{
  Serial.begin(115200);
  pinMode(startBtnPin, INPUT);
  pinMode(lightBarrierPin, INPUT);

  Serial.println("start machine");
}

void loop()
{

  switch (currentState) {
    case INIT:
      initSoftwareModules();
      delay(10);
      currentState = AWAIT_START_BTN;
      is_a_ready = false;
      is_b_ready = false;
      is_timeout = false;
      break;

    case AWAIT_START_BTN:
      if (startBtnPressed) {
        Serial.println("start_triggered");
        currentState = REQUEST_READY_STATUS;
        startBtnPressed = false;
        carA->request_ready_for_start();
        // carB->request_ready_for_start();
        startRequestReady_ms = millis();
      }
      break;
    
    case REQUEST_READY_STATUS:
      is_a_ready = carA->is_ready_for_start();
      // is_b_ready = carB->is_ready_for_start();
      is_timeout = false;

      if (millis()-startRequestReady_ms > requestReadyTimeout_ms){
        is_timeout = true;
      }

      if ((is_a_ready && is_b_ready) || ((is_a_ready || is_b_ready) && is_timeout)){
        Serial.println("cars are ready");
        currentState = PLAY_START_SEQUENCE;
        trigger_start();
      }else if (is_timeout)
      {
        Serial.println("timeout no car avilable");
        currentState = AWAIT_START_BTN;
      }
      
      break;

    case PLAY_START_SEQUENCE:
      if (play_start_sequence()) {
        Serial.println("start sequence_end");
        currentState = START_RACE;
        
      }
      break;

    case START_RACE:
      lap_display_start_timer();
      //goal start race
      currentState = RACE;
      break;

    case RACE:
      race();
      // if (startBtnPressed || action_server_is_canceled()) {
      if (startBtnPressed){
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

  checkButtonPress();
  carA->run_action();
  // carB->run_action();
}

void initSoftwareModules() {
  // Initialize software modules
  Serial.println("now init uros");
  init_uros();
  Serial.println("create Roscomm instances");

  carA = new RosComm("race", 25);
  // carB = new RosComm("raceB", 26);

  init_start_lights();
  lap_display_begin();
  lap_display_reset_timer();

  Serial.println("init done");

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

  if(millis() % 5000 == 0){
    Serial.print("carA curr pos: ");
    Serial.println(carA->get_current_position());
    // Serial.print("carB curr pos: ");
    // Serial.println(carB->get_current_position());
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
