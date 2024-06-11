#include "start_lights.h"

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(PIXEL_COUNT, DATA_PIN, CHANNEL, TYPE_GRB);

unsigned long StartTime = 0;
unsigned long RndmStartTime = 2000;
int oldPixelsToLightUp = 0;

bool is_led_on = false;

void init_start_lights()
{
	strip.begin();
	strip.setBrightness(255);
    strip.show();
    Serial.println("Start_lights");
}
void lights_off(){
    strip.setAllLedsColorData(0);
    strip.show();
}
bool play_start_sequence()
{
    unsigned long TimeIntoStart = millis() - StartTime;
    if (TimeIntoStart < NEXT_LIGHT_INTERVAL * MODULE_COUNT)
    {
        int ModulesToLightUp = TimeIntoStart / NEXT_LIGHT_INTERVAL + 1;
        int PixelsToLightUp = ModulesToLightUp * PIXEL_PER_MODULE;

        if(oldPixelsToLightUp != PixelsToLightUp){
            oldPixelsToLightUp = PixelsToLightUp;
            for (int i = 0; i < PixelsToLightUp; i++)
            {
                strip.set_pixel(i, 0, 255, 0);
                is_led_on = true;
            }
            strip.show();
        }
    }
    else if (TimeIntoStart < NEXT_LIGHT_INTERVAL * MODULE_COUNT + RndmStartTime)
    {
        delay(1);
        // wait for start
    }
    else
    {
        if(is_led_on){
            is_led_on = false;
            lights_off();
            
        }
        return true;
    }
    return false;
}

void signal_get_ready()
{
    strip.setAllLedsColorData(0, 0, 255);
    strip.show();
}

void trigger_start()
{
    lights_off();
    RndmStartTime = random(1000, 3000);
    StartTime = millis();
}