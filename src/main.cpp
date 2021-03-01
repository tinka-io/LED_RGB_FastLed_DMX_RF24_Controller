#include <Arduino.h>
#include "RGB_Stripe.hpp"
#include "RF24_reciver.hpp"

RGBstripe stripe;
RF24reciver rf = RF24reciver();

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to Tinkas RGB FastLed DMX RF24 Controller");

  stripe.setup();
  rf.setup();
}

void loop() {
  //stripe.loop_DMX(1);
  //stripe.loop_DMX_light(1);

  float ts = rf.get_timestamp();
  static float lts = -1;
  if (lts + 1 <= ts || (0 < ts && ts < 1) || ts == -2)
  {
    Serial.println(ts);
    lts = ts;
  }
  
  stripe.loop_timecode(ts);
}