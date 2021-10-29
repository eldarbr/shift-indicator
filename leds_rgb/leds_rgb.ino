#include "FastLED.h"
#define ledPIN 4
#define ledNUM 16

struct colorManager {
  byte color; //color X - hue in degrees
  byte maxValue; //max color brightness
  byte spot; //gradient position of the color X
};

//array (1)
colorManager colors[] = {{85, 45, 13}, {43, 100, 40}, {0, 91, 80}, {170, 90, 86}}; //set color points; the rest, transitions between set points, will be computed

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


//this function computes color map - static colors for each led between set color points (array (1))
byte SetColorMap(byte spot) {
  byte clN = sizeof(colors) / 3;
  if (spot >= colors[clN - 1].spot) {
    return colors[clN - 1].color;
  } else if (spot <= colors[0].spot) {
    return colors[0].color;
  } else {
    for (byte i = 0; i < (clN - 1); i++) {
      if (spot == colors[i].spot) {
        return colors[i].color;
      } else if (spot > colors[i].spot && spot < colors[i + 1].spot) {
        if (abs(colors[i].color - colors[i + 1].color) < abs(colors[i].color - (colors[i + 1].color - 255))) {
          return map(spot, colors[i].spot, colors[i + 1].spot, colors[i].color, colors[i + 1].color);
        } else {
          return map(spot, colors[i].spot, colors[i + 1].spot, colors[i].color, colors[i + 1].color - 255);
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
