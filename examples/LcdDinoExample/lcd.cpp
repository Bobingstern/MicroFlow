#include <Arduino.h>
#include <LiquidCrystal.h>
#include "pinouts.h"
#include "config.h"
#include "lcd.h"

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void initLCD() {
  lcd.begin(WIDTH, HEIGHT);
}

void initButtons() {
  pinMode(buttonPin, INPUT);
}

byte readButtons() {
  #if defined(USE_RESISTOR_LADDER)
  int buttonVal = analogRead(buttonPin);

  if (buttonVal < 10) {
    return RIGHT;
  } else if (buttonVal < 150) {
    return UP;
  } else if (buttonVal < 300) {
    return DOWN;
  } else if (buttonVal < 450) {
    return LEFT;
  } else if (buttonVal < 700) {
    return SELECT;
  } else {
    return NONE;
  }
  #else
  return digitalRead(buttonPin);
  #endif
}
