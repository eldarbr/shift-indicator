#include "FastLED.h"
#define ledPIN 4
#define ledNUM 16

struct colorManager {
  byte color; //hue in degrees
  byte maxValue; //max color brightness
  byte pose; //gradient position of the color X
};

//array (1)
colorManager colors[] = {{85, 45, 13}, {43, 100, 40}, {0, 91, 80}, {170, 90, 86}}; //set color points; the rest, transition between theese points, will be counted

byte ledsN = ledNUM; //precount positions (percents) for 16 leds [0 6 13 20 26 33 40 46 53 60 66 73 80 86 93 100]
byte ledsOn = 16;
int mxT = 1000;

unsigned long lastFill;

CRGB leds[ledNUM];
CHSV ledshsv[ledNUM];

void setup() {
  Serial.begin(115200);
  LedInit();
}

void loop() {
  if (Serial.available() > 0) {
    FillStrip(Serial.parseInt());
  }
}


void LedInit() {
  FastLED.addLeds<WS2811, ledPIN, GRB>(leds, ledsN).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(20);
  for (byte i = 0; i < ledsN; i++) {
    ledshsv[i] = CHSV(SetColorMap(round(i * 100 / 15)), 255, 255);
    hsv2rgb_spectrum(ledshsv[i], leds[i]);
  }
  FastLED.show();
}


//this function computes color map - static colors for each led among set color points (set in array (1))
byte SetColorMap(byte pose) {
  byte clN = sizeof(colors) / 3;
  if (pose >= colors[clN - 1].pose) {
    return colors[clN - 1].color;
  } else if (pose <= colors[0].pose) {
    return colors[0].color;
  } else {
    for (byte i = 0; i < (clN - 1); i++) {
      if (pose == colors[i].pose) {
        return colors[i].color;
      } else if (pose > colors[i].pose && pose < colors[i + 1].pose) {
        if (abs(colors[i].color - colors[i + 1].color) < abs(colors[i].color - (colors[i + 1].color - 255))) {
          return map(pose, colors[i].pose, colors[i + 1].pose, colors[i].color, colors[i + 1].color);
        } else {
          return map(pose, colors[i].pose, colors[i + 1].pose, colors[i].color, colors[i + 1].color - 255);
        }
      }
    }
  }
}



//attempt to implement a smooth transition animation between two led strip fill values 
void FillStrip(byte newFill) {
  byte cursr = ledsOn;
  if (newFill > ledsOn) {
    cursr -= 2;
  }
  int toFill = (newFill - ledsOn) * 256;
  byte delta = toFill / mxT;
  lastFill = millis() - mxT;
  bool changed;
  while (ledsOn != newFill) {
    changed = false;
    if (cursr > newFill && (millis() - lastFill) > mxT / (toFill / delta)) {
      if (leds[cursr].val <= delta) {
        ledsOn -= 1;
        cursr -= 1;
        leds[cursr].val = 0;
        changed = true;
      }
      else {
        leds[cursr].val -= delta;
      }
    } else if ((millis() - lastFill) > mxT / (toFill / delta)) {
      if (255 - leds[cursr].val <= delta) {
        ledsOn += 1;
        cursr += 1;
        leds[cursr].val = 255;
        changed = true;
      }
      else {
        leds[cursr].val += delta;
      }
    }
    if (changed) {
      FastLED.show();
    }
  }
}
