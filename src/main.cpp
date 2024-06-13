// TODO: 
// - reset car struct on restart
// - get timings straight in lap_display
//    - min time
//    - stop time

#include <Arduino.h>

#include "start_lights.h"
#include "lap_display.h"
#include "car_com.h"
#include "glob_defines.h"

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

CarCom* car_com = nullptr;

const int startBtnPin = 32;  // Pin connected to start button
bool startBtnPressed = false;

const int lightBarrierPin = 22;
bool lightBarrierTriggered = false;

unsigned long startRequestReady_ms = 0;
const unsigned long requestReadyTimeout_ms = 10000;

bool is_a_ready = false;
bool is_b_ready = false;
bool is_timeout = false;

int active_cars = 0;

unsigned long last_drivethrough_ms = 0;
unsigned long drivethrough_duration_ms = 1000;
// Function declarations
void initSoftwareModules();
bool playStartSequence();
void race();
void stopRace();
void checkButtonPress();

int current_car = 0;
bool car_abort = false;
bool airplain_mode = false;

void setup()
{
  Serial.begin(115200);
  pinMode(startBtnPin, INPUT);
  pinMode(lightBarrierPin, INPUT);

  Serial.printf("start machine\n");
  if (digitalRead(startBtnPin) == HIGH){
    Serial.println("Airplain mode is on");
    airplain_mode = true;
    while(digitalRead(startBtnPin) == HIGH){
      delay(100);
    }
  }
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
      car_com->reset();
      break;

    case AWAIT_START_BTN:
      if (startBtnPressed) {
        lap_display_reset_timer();
        signal_get_ready();
        Serial.println("start_triggered");
        currentState = REQUEST_READY_STATUS;
        startBtnPressed = false;
        Serial.println("request ready for start / send goal request");
        if(!airplain_mode){
          car_com->request_ready_for_start();
        }
        startRequestReady_ms = millis();

        Serial.println("now wait for cars");
      }
      break;
    
    case REQUEST_READY_STATUS:
      is_timeout = true;
      if(!airplain_mode){
        is_a_ready = car_com->get_status(CAR_A) == READY_TO_RACE;
        is_b_ready = car_com->get_status(CAR_B) == READY_TO_RACE;

        is_timeout = false;
      }

      if (millis()-startRequestReady_ms > requestReadyTimeout_ms){
        is_timeout = true;
      }

      if ((is_a_ready && is_b_ready) || ((is_a_ready || is_b_ready) && is_timeout)){
        Serial.println("cars are ready, now play start sequence");
        currentState = PLAY_START_SEQUENCE;
        active_cars = CAR_A*is_a_ready+CAR_B*is_b_ready;
        Serial.print("active car sum: ");
        Serial.println(active_cars);
        trigger_start();
      }else if (is_timeout)
      {
        Serial.println("timeout no car avilable");
        currentState = AWAIT_START_BTN;
        if(airplain_mode){
          currentState = PLAY_START_SEQUENCE;
          trigger_start();
        }
        lights_off();
      }
      
      break;

    case PLAY_START_SEQUENCE:
      if (play_start_sequence()) {
        Serial.println("start sequence_end");
        currentState = START_RACE;
      }
      break;

    case START_RACE:
      Serial.println("start race, send_result_request");
      //goal start race
      if(!airplain_mode){
        car_com->start_race();
      }
      
      currentState = RACE;
      Serial.println("now got to race state :)");
      break;

    case RACE:
      race();
      if(!airplain_mode){
        car_abort = car_com->is_race_aborted();
      }
      if (startBtnPressed || car_abort){
        Serial.println("end_race");
        currentState = STOP_RACE;
        startBtnPressed = false;
      }
      break;

    case STOP_RACE:
      display_best_times();
      if(!airplain_mode){
        car_com->send_cancel_request();
        car_com->reset();
      }

      currentState = AWAIT_START_BTN;
      break;
  }

  checkButtonPress();
  if(!airplain_mode){
    car_com->handle_car_com();
  }
}


void initSoftwareModules() {
  // Initialize software modules
  if(!airplain_mode){
    Serial.println("now init car communication via webhooks");
    car_com = new CarCom();
    Serial.println("add car ids");
    
    car_com->add_car_id(CAR_A);
    car_com->add_car_id(CAR_B);
  }

  init_start_lights();
  lap_display_begin();
  lap_display_reset_timer();

  Serial.println("init done");

}

int get_car_on_finish_line(){
  if(airplain_mode){
    // just toggle CAR A and CAR B
    if(CAR_B == -1){
      return CAR_A;
    }

    if(current_car == CAR_A){
      current_car = CAR_B;
    }else if(current_car == CAR_B){
      current_car = CAR_A;
    }else{
      current_car = CAR_A;
    }
    return current_car;

  }else{
    if(active_cars == CAR_A) return CAR_A; // car A isn't available
    if(active_cars == CAR_B) return CAR_B; // car B isn't available

    int pos_A = car_com->get_current_position(CAR_A);
    int pos_B = car_com->get_current_position(CAR_B);

    if(pos_A > pos_B)
      return CAR_A;
    else
      return CAR_B;
  }
}

void race() {
  // Race logic
  // Pseudo code: Measure lap times, display current lap, etc.
  
  if(digitalRead(lightBarrierPin) == HIGH && lightBarrierTriggered == false){
    int current_car = get_car_on_finish_line();
    Serial.print("car ");
    Serial.print(current_car);
    Serial.println(" has crossed the finish line");

    if(current_car == CAR_A){
      lap_display_start_new_lap_A();
    }else if(current_car == CAR_B){
      lap_display_start_new_lap_B();
    }
    last_drivethrough_ms = millis();
    lightBarrierTriggered = true;

  }

  if(drivethrough_duration_ms<millis()-last_drivethrough_ms){
    lightBarrierTriggered = false;
  }

  if(millis() % 5000 == 0 && !airplain_mode){
    Serial.print("carA curr pos: ");
    Serial.println(car_com->get_current_position(CAR_A));
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
