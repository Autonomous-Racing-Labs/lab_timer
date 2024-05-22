#include "start_lights.h"

NeoPixelBus<NeoGrbwFeature, NeoWs2812xMethod> strip(PIXEL_COUNT, DATA_PIN);

unsigned long StartTime = 0;
unsigned long RndmStartTime = 2000;

void init_start_lights()
{
    strip.Begin();
    strip.Show();
}

bool play_start_sequence()
{
    unsigned long TimeIntoStart = millis() - StartTime;
    if (TimeIntoStart < NEXT_LIGHT_INTERVAL * MODULE_COUNT)
    {
        int ModulesToLightUp = TimeIntoStart / NEXT_LIGHT_INTERVAL + 1;
        int PixelsToLightUp = ModulesToLightUp * PIXEL_PER_MODULE;

        for (int i = 0; i < PixelsToLightUp; i++)
        {
            strip.SetPixelColor(i, RgbwColor(255, 0, 0, 0));
        }
        strip.Show();
    }
    else if (TimeIntoStart < NEXT_LIGHT_INTERVAL * MODULE_COUNT + RndmStartTime)
    {
        delay(1);
        // wait for start
    }
    else
    {
        for (int i = 0; i < strip.PixelCount(); i++)
        {
            strip.SetPixelColor(i, RgbwColor(0, 0, 0, 0));
        }
        strip.Show();
        return true;
    }
    return false;
}

void trigger_start()
{
    StartTime = millis();
}