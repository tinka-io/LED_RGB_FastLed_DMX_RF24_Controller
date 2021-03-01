#ifndef RGBSTRIPE_H
#define RGBSTRIPE_H

#include <TeensyDMX.h>
namespace teensydmx = ::qindesign::teensydmx;

//#######################################################################

#define FASTLED_ALLOW_INTERRUPTS 0
//#define FASTLED_INTERRUPT_RETRY_COUNT 1
#define USE_OCTOWS2811
#include <OctoWS2811.h>
#include <FastLED.h>
FASTLED_USING_NAMESPACE
#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_TYPE WS2812
#define COLOR_ORDER GRB
// Lang 34 + Kurz 22 = 56 -> x2
//    K 34 +    K 34 = 68
// Gesamt 180

#define NUM_LEDS_PER_STRIP 68
#define NUM_STRIPS 2
const int NUM_LEDS = NUM_STRIPS * NUM_LEDS_PER_STRIP;

#define FRAMES_PER_SECOND 60
//#######################################################################

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

struct FrameS
{
    u8 fm;
    u8 to;
    CRGB color;
    u8 dim;
};

class RGBstripe
{
public:
    void setup();
    void loop_StatusLED();
    void loop_DMX(int dmxStart);
    void loop_DMX_ligh(int dmxStart);
    void loop_timecode(float time);

private:
    teensydmx::Receiver dmxRx{Serial2};
    FrameS frame[6];
    CRGB leds[NUM_LEDS];
    void leds_gamma();

    CRGB interpolate(CRGB startC, CRGB endC, int stepNumber, int lastStepNumber);
    void set_part_color(int b, int e, CRGB c1, CRGB c2, int pos, int Q);
    void set_frame_edge_color(CRGB OL, CRGB OR, CRGB UR, CRGB UL, int V, int H, int Q, int mode);
    void color_part(int from, int to, CRGB color);
    void color_parts(float freq, CRGB c1, CRGB c2, CRGB c3, CRGB c4);
    void rainbow(u8 gHue);
    void addGlitter(fract8 chanceOfGlitter);
    void confetti(u8 Q);
    void sinelon(u8 Q, u8 H);
    void bpm(u8 gHue, float BeatsPerMinute);
    void juggle();
    void simple_color();
    void area_dim();

    // for timecode show
public:
    int reset_BPM(int i)
    {
        static u8 lasti = 0;
        if (lasti != i)
        {
            resetBPM = true;
            lasti = i;
        }
        return lasti;
    }

private:
    bool resetBPM = false;

    void set_color(CRGB color);
    void setFrameColor(CRGB color);
    void colorFillsmooth(CRGB c, uint32_t bpm_ms, uint32_t curr_time, FrameS f, int fade, bool ccw); // fipc -> fade in pixel count
    void shuffleon(float bpm);
    void fade_color(float bpm, CRGB color, bool dirout);
    void strobe(float bpm, CRGB color);
    void fade_from_middle(float bpm, CRGB color, float fade);
    void fade_a_round(float bpm, CRGB color, float fade);
    void loop_color_glitter(fract8 chanceOfGlitter, CRGB color, u8 fade);
};

#endif