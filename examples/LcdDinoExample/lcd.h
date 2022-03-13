#include <Arduino.h>
#include <LiquidCrystal.h>

const byte WIDTH = 16;
const byte HEIGHT = 2;

extern LiquidCrystal lcd;

const byte NONE = 0;
const byte SELECT = 1;
const byte UP = 2;
const byte LEFT = 3;
const byte DOWN = 4;
const byte RIGHT = 5;

void initLCD();

void initButtons();
byte readButtons();
