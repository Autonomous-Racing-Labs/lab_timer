#include <Arduino.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

#define LED_BUILTIN 13
#define NUM_LEDS 7
#define DATA_PIN 13


const uint16_t PixelCount = 7; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = DATA_PIN;  // make sure to set this to the correct pin, ignored for Esp8266
const uint16_t AnimCount = 1; // we only need one
const uint16_t TailLength = 1; // length of the tail, must be shorter than PixelCount
const float MaxLightness = 0.4f; // max lightness at the head of the tail (0.5f is full bright)

NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

NeoPixelBus<NeoGrbwFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);
// for esp8266 omit the pin
//NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount);

NeoPixelAnimator animations(AnimCount); // NeoPixel animation management object


void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        strip.RotateRight(1);
    }
}

void DrawTailPixels()
{
    // using Hsl as it makes it easy to pick from similiar saturated colors
    float hue = 200;
    for (uint16_t index = 0; index < strip.PixelCount() && index <= TailLength; index++)
    {
        float lightness = index * MaxLightness / TailLength;
        RgbColor color = RgbColor(255,255,255);

        strip.SetPixelColor(index, colorGamma.Correct(color));
    }
}

void setup()
{
    strip.Begin();
    strip.Show();


    // we use the index 0 animation to time how often we rotate all the pixels
    animations.StartAnimation(0, 66, LoopAnimUpdate); 
}


void loop()
{
    // this is all that is needed to keep it running
    // and avoiding using delay() is always a good thing for
    // any timing related routines
    //animations.UpdateAnimations()

    for (int i = 0; i < strip.PixelCount(); i++)
    {

      for (int j = 0; j < strip.PixelCount(); j++){
        strip.SetPixelColor(j, RgbwColor(0, 255, 0, 0));
      }
    strip.SetPixelColor(i, RgbwColor(255, 0, 0, 0));
      strip.Show();
      delay(200);

    }
    

}
