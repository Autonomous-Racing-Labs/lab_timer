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
