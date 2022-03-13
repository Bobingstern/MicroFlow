#include <Arduino.h>

extern unsigned int gameTickDelay;

extern byte gameTimeInAir;

extern byte lastCactusTick;

extern unsigned long score;

extern char field[WIDTH];

void gameInit();

void startScreen();
void endScreen();

bool gameTick();
void gameJump();

void shiftFieldLeft();
void drawField();
bool timeForCactus();
byte distanceToNextCacti();

void seedRNG();
byte widthOfNum(unsigned long num);
