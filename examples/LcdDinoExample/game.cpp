#include <Arduino.h>
#include "lcd.h"
#include "bitmaps.h"
#include "config.h"
#include "game.h"

#if defined(STARTING_TICK_DELAY)
unsigned int gameTickDelay = STARTING_TICK_DELAY;
#else
unsigned int gameTickDelay = 500;
#endif
byte gameTimeInAir = 0;

byte lastCactusTick = 0;

unsigned long score = 0;

char field[WIDTH];

void gameInit() {
  #if defined(SERIAL_DISTANCE_CACTI)
  //Serial.begin(9600);
  #endif
  // Doesn't like const byte* so we cast to byte*
  lcd.createChar(DINO, (byte*)dino);
  lcd.createChar(CACTUS, (byte*)cactus);
  lcd.clear();
  lcd.noCursor();
  memset(field, SPACE, WIDTH);
  seedRNG();
}

void startScreen() {
  lcd.home();
  lcd.println("Chrome Dino game");
  lcd.setCursor(dinoX, HEIGHT - 1);  // Last row
  lcd.write(DINO);

  while (readButtons() == NONE) {
    ;
  }
}

void endScreen() {
  lcd.home();
  lcd.print("Game over");
  lcd.setCursor(WIDTH - widthOfNum(score), 0);
  lcd.print(score);
  drawField();
}

bool gameTick() {
  lcd.clear();
  shiftFieldLeft();
  if (field[dinoX] != SPACE && gameTimeInAir == 0) {
    #if defined(SERIAL_DISTANCE_CACTI)
    Serial.println(0);
    #endif
    field[dinoX] = CACTUS;
    return false;
  }
  if (timeForCactus()) {
    field[WIDTH - 1] = CACTUS;
  }
  if (gameTimeInAir > 0) {
    if (field[dinoX] == DINO) {
      field[dinoX] = SPACE;
    }
    gameTimeInAir --;
  } else {
    field[dinoX] = DINO;
  }
  drawField();
  lcd.setCursor(dinoX, HEIGHT - 2);
  if (gameTimeInAir > 0) {
    lcd.write(DINO);
  } else {
    lcd.write(SPACE);
    lcd.setCursor(dinoX, HEIGHT - 1);
    lcd.write(DINO);
  }
  score ++;
  lcd.setCursor(WIDTH - widthOfNum(score), 0);
  lcd.print(score);
  #if defined(SERIAL_DISTANCE_CACTI)
//  if (gameTimeInAir > 0) {
//    Serial.println(-1);
//  } else {
//    Serial.println(distanceToNextCacti());
//  }
  #endif
  return true;
}

void gameJump() {
  if (gameTimeInAir != 0) {
    return;
  }
  gameTimeInAir = timeInAir;
}

void shiftFieldLeft() {
  // Normally we would store first value in temporary variable
  // But we don't need it here
  for (byte i = 0; i < WIDTH - 1; i ++) {
    if (field[i + 1] == DINO) {
      continue;
    }
    field[i] = field[i + 1];
  }
  field[WIDTH - 1] = SPACE;
}

void drawField() {
  lcd.setCursor(0, HEIGHT - 1);
  for (char bit : field) {
    lcd.write(bit);
  }
}

bool timeForCactus() {
  lastCactusTick ++;
  if (lastCactusTick <= minCactiGap) {
    return false;
  }
  if (random(chanceOfCacti) == 0) {
    lastCactusTick = 0;
    return true;
  } else {
    return false;
  }
}

byte distanceToNextCacti() {
  byte distance = 0;
  for (byte i = dinoX; i < WIDTH; i ++) {
    if (field[i] == CACTUS) {
      break;
    }
    distance ++;
  }
  return distance;
}

void seedRNG() {
  #if defined(CONSTANT_SEED)
  randomSeed(CONSTANT_SEED);
  #else
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  randomSeed((unsigned long)analogRead(A1) * (unsigned long)analogRead(A2));
  #endif
}

byte widthOfNum(unsigned long num) {
  byte width = 0;
  unsigned long x = num;
  while (x != 0) {
    x = x / 10;
    width ++;
  }
  return width;
}
