#include <RGB_Stripe.hpp>

u8 led_gamma[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
    5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
    10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
    17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
    25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
    37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
    51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
    69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
    90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
    115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
    144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
    177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
    215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255};

void RGBstripe::leds_gamma()
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].r = led_gamma[leds[i].r];
        leds[i].g = led_gamma[leds[i].g];
        leds[i].b = led_gamma[leds[i].b];
    }
}

void RGBstripe::setup()
{
    // set color areas
    /*
    frame[0].fm = 0 * 68 + 0;   // long
    frame[0].to = 0 * 68 + 34;

    frame[1].fm = 0 * 68 + 34;  // short
    frame[1].to = 0 * 68 + 34 + 22;
    */

    frame[0].fm = 0 * 68 + 0; // long
    frame[0].to = 0 * 68 + 22;

    frame[1].fm = 0 * 68 + 22; // short
    frame[1].to = 0 * 68 + 22 + 34;

    /* One Stribe
    frame[2].fm = 34+22;
    frame[2].to = 34+22+34;

    frame[3].fm = 34+22+34;
    frame[3].to = 34+22+34+22;
  */

    frame[2].fm = 1 * 68;
    frame[2].to = 1 * 68 + 34;

    frame[3].fm = 1 * 68 + 34;
    frame[3].to = 1 * 68 + 34 + 22;

    /* One Stribe  L +S +L +S +K +K
    frame[4].fm = 34+22+34+22;
    frame[4].to = 34+22+34+22+34;
    frame[5].fm = 34+22+34+22+34;
    frame[5].to = 34+22+34+22+34+34;
  */

    frame[4].fm = 2 * 68;
    frame[4].to = 2 * 68 + 34;
    frame[5].fm = 2 * 68 + 34;
    frame[5].to = 2 * 68 + 34 + 34;

    // init DMX
    dmxRx.begin();

    dmxRx.onConnectChange([](teensydmx::Receiver *r) {
        digitalWriteFast(LED_BUILTIN, r->connected() ? HIGH : LOW);
    });

    // init OCTOWS2811
    LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);

    // init Fast LED
    //FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)//.setCorrection(TypicalLEDStrip);
    //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    //FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);//.setCorrection(TypicalLEDStrip);
    //FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}
void RGBstripe::loop_StatusLED()
{
}

void RGBstripe::loop_DMX(int dmxStart)
{
    constexpr uint32_t kTimeout = 1000; // In milliseconds
    if (millis() - dmxRx.lastPacketTimestamp() > kTimeout)
    {
        {
            u8 dmxBuffer[NUM_LEDS_PER_STRIP * 2 * 3];
            int read = dmxRx.readPacket(dmxBuffer, dmxStart, NUM_LEDS_PER_STRIP * 2 * 3);
            if (read > 0)
            {
                int x = 0;
                for (int i = 0; i < NUM_LEDS_PER_STRIP * 2; i++)
                {
                    uint8_t r = dmxBuffer[3 * i + 0 + dmxStart - 1];
                    uint8_t g = dmxBuffer[3 * i + 1 + dmxStart - 1];
                    uint8_t b = dmxBuffer[3 * i + 2 + dmxStart - 1];
                    leds[x] = CRGB(r, b, g);

                    // go on with Chanel 2
                    if (x == 55)
                    {
                        x = NUM_LEDS_PER_STRIP;
                    }
                    else
                    {
                        x++;
                    }
                }

                LEDS.setBrightness(255);
                LEDS.show();
            }
        }
    }
}

void RGBstripe::loop_DMX_ligh(int dmxStart)
{
    constexpr uint32_t kTimeout = 1000; // In milliseconds
    if (millis() - dmxRx.lastPacketTimestamp() > kTimeout)
    {
        {
            u8 dmxBuf[NUM_LEDS_PER_STRIP * 2 * 3];
            int read = dmxRx.readPacket(dmxBuf, dmxStart, NUM_LEDS_PER_STRIP * 2 * 3);
            if (read > 0)
            {
                // shift the dmxBuffer down from DMX start address
                /*
                    u8 dmxBuf[29];
                    for (int i = 0; i < 29; i++)
                    {
                        dmxBuf[i] = dmxBuffer[i + dmxStart - 1];
                    }*/

                // get the areas color & dimmer
                for (int x = 0; x < 6; x++)
                {
                    frame[x].color = CRGB(dmxBuf[4 * x], dmxBuf[4 * x + 2], dmxBuf[4 * x + 1]);
                    frame[x].dim = dmxBuf[4 * x + 3];
                    //Serial.print("x:" + String(x) + "\t" + " " + String(frame[x].color[0]) + " " + String(frame[x].color[1]) + " " + String(frame[x].color[2]) + "  "+ String(frame[x].dim) + "\t");
                }
                //Serial.println();

                // get the gloabl controls
                static u8 H = dmxBuf[25 - 1];
                static u8 V = dmxBuf[26 - 1];
                static u8 M = dmxBuf[27 - 1];
                static u8 Q = dmxBuf[28 - 1];
                static u8 dim = dmxBuf[29 - 1];

                // the different Modes
                if (0 <= M && M <= 30)
                {
                    simple_color();
                }
                if (30 < M && M <= 60)
                {
                    confetti(Q);
                }
                if (60 < M && M <= 90)
                {
                    sinelon(Q, H);
                }
                if (90 < M && M <= 120)
                    bpm(Q, H);
                if (120 < M && M <= 150)
                    ;
                if (150 < M && M <= 180)
                    juggle();
                if (180 < M && M <= 210) //set_frame_edge_color(colors[0], colors[1], colors[2], colors[3], V, H, Q, M);
                    if (210 < M && M <= 256)
                        fill_rainbow(leds, NUM_LEDS, V, 7);

                // dim the areas
                area_dim();

                LEDS.setBrightness(dim);
                LEDS.show();
                //LEDS.delay(1000 / FRAMES_PER_SECOND);
            }
        }
    }
}

void RGBstripe::loop_timecode(float ts)
{
    CRGB dgreen = CRGB(0, 0, 100);
    CRGB green = CRGB(0, 0, 255);
    CRGB cyan = CRGB(0, 100, 255);
    CRGB mage = CRGB(25, 100, 0);
    CRGB blue = CRGB(0, 255, 0);
    CRGB red = CRGB(255, 0, 0);
    CRGB yellow = CRGB(200, 0, 150);
    CRGB black = CRGB(0, 0, 0);

    //ts += 238;

    /*
    set_color(cyan);
    LEDS.setBrightness(255);
    LEDS.show();
    return;
    */

    float o = -1;

    static double chilltime = 0;
    static u8 b = 0;
    if (ts == -2)
    {
        if (chilltime == -1)
        {
            chilltime = millis();
        }

        if (b < 255)
        {
            b = map(millis(), chilltime, chilltime + 750, 0, 255);
            LEDS.setBrightness(b);
            set_color(cyan);
        }
    }
    else if (ts == -3)
    {
        if (chilltime == -1)
        {
            chilltime = millis();
        }

        if (b > 0)
        {
            b = map(millis(), chilltime, chilltime + 250, 255, 0);
            LEDS.setBrightness(b);
            set_color(cyan);
        }
    }
    else
    {
        chilltime = -1;

        if (ts < 0.3)
        {
            set_color(CRGB(0, 0, 0));
            LEDS.setBrightness(10);
        }
        else if (ts < 56)
        {
            reset_BPM(1);
            fade_from_middle(27.27 * 2, blue, 10);

            if (ts < 20)
            {
                u8 b = map(ts, 0, 20, 10, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 47)
            {
            }
            else if (ts < 53)
            {
                u8 b = map(ts, 47, 53, 255, 0);
                LEDS.setBrightness(b);
            }
        }
        else if (ts < 94)
        {
            LEDS.setBrightness(255);
            fade_color(27.53, cyan, true);
            //loop_color_glitter(54, black, 20);
        }
        else if (ts < 108)
        {
            set_color(CRGB(0, 0, 0));
        }
        else if (ts < 170.5) // Glitter 1
        {
            CRGB color = CRGB(0, random8(), random8());
            u8 chance = map(ts, 108, 118, 0, 12);
            loop_color_glitter(chance, color, 20);

            if (ts < 118)
            {
                u8 b = map(ts, 108, 118, 0, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 123)
            {
            }
            else if (ts < 131)
            {
                u8 b = map(ts, 123, 131, 255, 0);
                LEDS.setBrightness(b);
            }
            else
            {
                set_color(CRGB(0, 0, 0));
            }
        }
        else if (ts < 209 + o) // Glitter 2
        {
            CRGB color = CRGB(0, 255, 255);
            loop_color_glitter(64, color, 40);

            if (ts < 206 + o)
            {
                LEDS.setBrightness(255);
            }
            else
            {
                u8 b = map(ts, 206, 208, 255, 0);
                LEDS.setBrightness(b);
            }
        }
        else if (ts < 223.4 + o)
        {
            LEDS.setBrightness(255);
            fade_color(260, mage, false);
        }
        else if (ts < 236.8 + o)
        {
            LEDS.setBrightness(0);
        }
        else if (ts < 254.4 + o)
        {
            if (ts < 253.8 + o)
            {
                strobe(659, CRGB(255, 100, 0));
            }

            if (ts < 238.5 + o)
            {
                u8 b = map(ts, 236.8 + o, 238.5 + o, 128, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 239 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 240.8 + o)
            {
                u8 b = map(ts, 239 + o, 240.8 + o, 128, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 241.1 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 242.9 + o)
            {
                u8 b = map(ts, 241.1 + o, 242.9 + o, 128, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 243.2 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 244.9 + o)
            {
                u8 b = map(ts, 243.2 + o, 244.9 + o, 128, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 249.6 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 251.3 + o)
            {
                u8 b = map(ts, 249.6 + o, 251.3 + o, 128, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 251.8 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 253.8 + o)
            {
                u8 b = map(ts, 251.8 + o, 253.8 + o, 128, 255);
                LEDS.setBrightness(b);
            }
            else if (ts < 253.9 + o)
            {
                set_color(red);
            }
            else if (ts < 254.4 + o)
            {
                u8 b = map(ts, 253.9 + o, 254.3 + o, 255, 0);
                LEDS.setBrightness(b);
            }
        }
        else if (ts < 351.9 + o)
        {
            if (ts < 261.8 + o)
            {
                set_color(black);
                LEDS.setBrightness(255);
            }
            else if (ts < 266.4 + o)
                fade_color(659 / 2, blue, true);
            else if (ts < 273.3 + o)
                set_color(black);

            else if (ts < 274.9 + o)
            {
            }
            else if (ts < 280.8 + o)
                fade_color(659 / 2, blue, true);
            else if (ts < 289 + o)
                set_color(black);

            else if (ts < 295.5 + o)
                fade_color(659 / 2, blue, true);
            else if (ts < 303.3 + o)
            {
                set_color(black);
            }

            else if (ts < 304.6 + o)
                fade_color(659 / 2, blue, true);
            else if (ts < 311.2 + o)
            {
                set_color(black);
            }

            else if (ts < 320.2 + o)
                fade_color(659 / 2, blue, true);
            else if (ts < 320 + o)
            {
                fade_color(659 / 2, blue, true);
                u8 b = map(ts, 319 + o, 320 + o, 255, 0);
                LEDS.setBrightness(b);
            }
            else if (ts < 324 + o)
            {
                set_color(black);
            }
            else if (ts < 330 + o)
            {
                fade_color(300 / 2, blue, true);
                u8 b = map(ts, 324 + o, 330 + o, 0, 128);
                LEDS.setBrightness(b);
            }
            else if (ts < 335.9 + o)
            {
                fade_color(659 / 2, blue, true);
                u8 b = map(ts, 330 + o, 335.8 + o, 255, 0);
                LEDS.setBrightness(b);
            }
            else if (ts < 351.8)
            {
                LEDS.setBrightness(0);
            }
        }
        else if (ts < 365.8 + o)
        {
            reset_BPM(2);
            fade_from_middle(27.27, red, 10);

            if (ts < 354.7 + o)
            {
                u8 b = map(ts, 351.9 + o, 354.6 + o, 255, 0);
                LEDS.setBrightness(b);
            }
            else
            {
                LEDS.setBrightness(0);
            }
        }
        else if (ts < 396.2 + o)
        {
            if (ts < 367.9 + o)
            {
                set_color(CRGB(0, 255, 0));
                u8 b = map(ts, 365.8 + o, 367.8 + o, 255, 0);
                LEDS.setBrightness(b);
            }
            else if (ts < 375.1 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 377.2 + o)
            {
                set_color(CRGB(0, 255, 64));
                u8 b = map(ts, 375.1 + o, 377.1 + o, 255, 0);
                LEDS.setBrightness(b);
            }
            else if (ts < 384.8 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 386.9 + o)
            {
                set_color(CRGB(0, 255, 128));
                u8 b = map(ts, 384.8 + o, 386.8 + o, 255, 0);
                LEDS.setBrightness(b);
            }
            else if (ts < 394.1 + o)
            {
                LEDS.setBrightness(0);
            }

            else if (ts < 396.1 + o)
            {
                set_color(CRGB(0, 255, 255));
                u8 b = map(ts, 394.1 + o, 396.1 + o, 255, 0);
                LEDS.setBrightness(b);
            }
        }
        else if (ts < 404.5 + o) // Wirbel
        {
            LEDS.setBrightness(255);
            float bpm = map(ts, 396.2 + o, 405 + o, 64, 300);
            u8 g = map(ts, 396.2 + o, 405 + o, 0, 255);
            fade_a_round(bpm, CRGB(0, 255, g), 64);
        }
        else
        {
            LEDS.setBrightness(0);
        }
    }

    //leds_gamma();
    LEDS.show();
    //LEDS.delay(1000 / FRAMES_PER_SECOND);
}

CRGB RGBstripe::interpolate(CRGB startC, CRGB endC, int stepNumber, int lastStepNumber)
{
    //u8 sC[3] = { (u8)(startC >> 16), (u8)(startC >> 8), (u8)(startC) };
    //u8 eC[3] = { (u8)(  endC >> 16), (u8)(  endC >> 8), (u8)(  endC) };

    //Serial.print("u32:"+String(startC)+" R:"+String(sC[0])+" G:"+String(sC[1])+" B:"+String(sC[2])+"\n");

    CRGB color = CRGB(0, 0, 0);
    for (int i = 0; i < 3; i++)
    {
        color[i] = (endC[i] - startC[i]) * stepNumber / lastStepNumber + startC[i];
    }

    //CRGB color = CRGB(0,0,0);
    //color  = ((u32)c[0] << 16) + ((u32)c[1] <<  8) + (u32)c[2];

    //Serial.print("u32:"+String(color)+" R:"+String(c[0])+" G:"+String(sC[1])+" B:"+String(sC[2])+"\n");

    return color;
}

void RGBstripe::set_part_color(int b, int e, CRGB c1, CRGB c2, int pos, int Q)
{
    int steps = e - b;
    float os = (float)steps / (float)256;
    int fadeB = round(os * ((float)pos - (float)Q / (float)2));
    int fadeE = round(os * ((float)pos + (float)Q / (float)2));
    if (fadeE > steps)
        fadeE = steps;
    int fadeS = fadeE - fadeB;

    //Serial.print("steps:"+String(steps)+" os:"+String(os, 8)+" pos:"+String(pos)+" Q:"+String(Q)+" fadeB:"+String(fadeB)+" fadeE:"+String(fadeE)+" fadeS:"+String(fadeS)+"\n");

    for (int i = 0; i < steps; i++)
    {
        CRGB color = CRGB(0, 0, 0);
        if (i <= fadeB)
            color = c1;
        if (fadeB < i && i < fadeE)
            color = interpolate(c1, c2, i - fadeB, fadeS);
        if (fadeE <= i)
            color = c2;

        //if(i == steps-1) c = 0xFFFF;
        leds[b + i] = color;
    }
}

void RGBstripe::set_frame_edge_color(CRGB OL, CRGB OR, CRGB UR, CRGB UL, int V, int H, int Q, int mode)
{
    set_part_color(0, 34, OL, OR, V, Q);
    set_part_color(34, 56, OR, UR, H, Q);
    set_part_color(56, 90, UR, UL, 256 - V, Q);
    set_part_color(90, 112, UL, OL, 256 - H, Q);
}

void RGBstripe::color_part(int from, int to, CRGB color)
{
    for (from = from; from < to; from++)
    {
        leds[from] = color;
    }
}

void RGBstripe::color_parts(float freq, CRGB c1, CRGB c2, CRGB c3, CRGB c4)
{
    color_part(0, 36, c1);
    color_part(36, 50, c2);
    color_part(50, 86, c3);
    color_part(86, 100, c4);

    delay(1000 / freq);

    static int count = 0;
    if (count < 4)
    {
        count++;
        CRGB t = c4;
        c4 = c3;
        c3 = c2;
        c2 = c1;
        c1 = t;
        color_parts(freq, c1, c2, c3, c4);
    }
    count = 0;
}

void RGBstripe::rainbow(u8 gHue)
{
    fill_rainbow(leds, NUM_LEDS, gHue, 7);
}

void RGBstripe::addGlitter(fract8 chanceOfGlitter)
{
    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] += CRGB::White;
    }
}

void RGBstripe::confetti(u8 Q)
{
    u8 smooth = round((float)Q / (float)0xFF * 30.00);
    if (Q == 0)
    {
        smooth = 10;
    }
    fadeToBlackBy(leds, NUM_LEDS, smooth);

    int pos = random16(NUM_LEDS);
    for (int x = 0; x < 6; x++)
    {
        int f = frame[x].fm;
        int t = frame[x].to;
        if (f <= pos && pos < t)
        {
            leds[pos] += frame[x].color;
        }
    }
}

void RGBstripe::set_color(CRGB color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = color;
    }
}
void RGBstripe::setFrameColor(CRGB color)
{
    for (int x = 0; x < 6; x++)
    {
        frame[x].color = color;
    }
}

void RGBstripe::sinelon(u8 Q, u8 H)
{
    u8 fade = round((float)Q / (float)0xFF * 30.00);
    if (Q == 0)
    {
        fade = 10;
    }

    u8 bpm = round((float)H / (float)0xFF * 30.00);
    if (H == 0)
    {
        bpm = 15;
    }

    fadeToBlackBy(leds, NUM_LEDS, fade);
    int pos = beatsin16(bpm, 0, NUM_LEDS - 1);
    for (int x = 0; x < 4; x++)
    {
        int f = frame[x].fm;
        int t = frame[x].to;
        if (f <= pos && pos < t)
        {
            leds[pos] += frame[x].color;
        }
    }
}

void RGBstripe::colorFillsmooth(CRGB c, uint32_t bpm_ms, uint32_t curr_time, FrameS f, int fade, bool ccw) // fipc -> fade in pixel count
{
    int numled = (f.to - f.fm);
    int AllSteps = numled * 256 + 256 * fade;
    int CurrStep = (millis() - curr_time) / double(bpm_ms) * AllSteps;
    int CurrLED = CurrStep / 256;

    CRGB pix_col;
    for (int i = 0; i < numled; i++)
    {
        int lastfadeled = CurrLED - fade;

        // the fade windows go under zero
        if (lastfadeled < 0)
        {
        }
        // LED behind the fade window
        if (i < lastfadeled)
        {
            pix_col = CRGB(0, 0, 0);
        }
        else
        {
            // LED in fron of Current Step
            if (CurrLED < i)
            {
                pix_col = CRGB(0, 0, 0);
            }
            else // LED get fade val
            {
                float f = float(CurrStep - i * 256) / float(fade * 256);
                float r = f * float(c.r);
                float g = f * float(c.g);
                float b = f * float(c.b);
                pix_col = CRGB(r, g, b);
                /*
                if(r < 50 && b > 0){
                    Serial.println(String(CurrStep) + " - " + String(i) + "*256 / " + String(fade) + " * 256 * " + String(r) + " | " +  String(b));
                }*/
            }
        }

        // set Pixel
        if (ccw)
        {
            leds[f.to - 1 - i] = pix_col;
        }
        else
        {
            leds[f.fm + i] = pix_col;
        }
    }
}

void RGBstripe::fade_from_middle(float bpm, CRGB color, float fade)
{
    float bpm_ms = 60000 / bpm;
    static float startTime;
    static bool beatoff = true;
    if (startTime + bpm_ms < millis() || resetBPM)
    {
        resetBPM = false;
        startTime = millis();
        beatoff = !beatoff;
    }

    if (beatoff)
    {
        colorFillsmooth(color, bpm_ms, startTime, frame[0], fade, true);
        colorFillsmooth(color, bpm_ms, startTime, frame[1], fade, false);
        colorFillsmooth(color, bpm_ms, startTime, frame[2], fade, true);
        colorFillsmooth(color, bpm_ms, startTime, frame[3], fade, false);
    }
}

void RGBstripe::fade_a_round(float bpm, CRGB color, float fade)
{
    float bpm_ms = 60000 / bpm;
    static float startTime;
    //static bool beatoff = true;
    if (startTime + bpm_ms < millis() || resetBPM)
    {
        resetBPM = false;
        startTime = millis();
        // beatoff = !beatoff;
    }

    //if (beatoff)
    FrameS f;
    f.fm = 0;
    f.to = NUM_LEDS;
    colorFillsmooth(color, bpm_ms, startTime, f, fade, false);
}

void RGBstripe::shuffleon(float bpm)
{
    float bpm_ms = 60000 / bpm;
    static float startTime;
    static u8 onpart = 0;
    if (startTime + bpm_ms < millis() || resetBPM)
    {
        resetBPM = false;
        startTime = millis();
        onpart = map(random8(), 0, 255, 1, 4);
    }

    for (int i = 0; i < 4; i++)
    {
        if (i != onpart)
        {
            color_part(frame[i].fm, frame[i].to, CRGB(0, 0, 0));
        }
    }
}

void RGBstripe::fade_color(float bpm, CRGB color, bool dirout)
{
    float bpm_ms = 60000 / bpm;
    static float startTime;
    if (startTime + bpm_ms < millis() || resetBPM)
    {
        resetBPM = false;
        startTime = millis();
    }

    float fade = 1.0 / float(bpm_ms / (millis() - startTime));
    if (dirout)
    {
        fade = 1 - fade;
    }

    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i].r = color.r * fade;
        leds[i].g = color.g * fade;
        leds[i].b = color.b * fade;
    }
}

void RGBstripe::strobe(float bpm, CRGB c)
{
    float bpm_ms = 60000 / bpm;
    static float startTime;
    if (startTime + bpm_ms < millis() || resetBPM)
    {
        resetBPM = false;
        startTime = millis();
    }

    if (startTime + bpm_ms / 3 < millis())
    {
        set_color(c);
    }
    else
    {
        set_color(CRGB(0, 0, 0));
    }
}

void RGBstripe::loop_color_glitter(fract8 chanceOfGlitter, CRGB color, u8 fade)
{
    static u32 last;
    if (millis() - last > 30)
    {
        last = millis();
        fadeToBlackBy(leds, NUM_LEDS, fade);
    }

    if (random8() < chanceOfGlitter)
    {
        leds[random16(NUM_LEDS)] = color;
    }
}

void RGBstripe::bpm(u8 gHue, float BeatsPerMinute)
{
    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8(BeatsPerMinute, 64, 255);
    for (int i = 0; i < NUM_LEDS; i++)
    { //9948
        leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
    }
}

void RGBstripe::juggle()
{
    // eight colored dots, weaving in and out of sync with each other
    fadeToBlackBy(leds, NUM_LEDS, 20);
    byte dothue = 0;
    for (int i = 0; i < 8; i++)
    {
        leds[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
        dothue += 32;
    }
}

void RGBstripe::simple_color()
{
    for (int x = 0; x < 6; x++)
    {
        int f = frame[x].fm;
        int t = frame[x].to;
        for (; f < t; f++)
        {
            leds[f] = frame[x].color;
        }
    }
}

void RGBstripe::area_dim()
{
    for (int x = 0; x < 6; x++)
    {
        int f = frame[x].fm;
        int t = frame[x].to;
        for (f = f; f < t; f++)
        {
            for (int i = 0; i < 3; i++)
            {
                leds[f][i] = round((float)leds[f][i] / (float)0xFF * (float)frame[x].dim);
            }
        }
    }
}