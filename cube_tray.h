#include <Arduino.h>
#include "pinout.h"

void takeInventory();

void dispenseCube();

int findCube(int bit3, int bit2);

void rotate(int deg);

void pushCube();

bool checkMagnetic();
bool checkMetal();
bool checkClear();

void printCubes();
void printCubeBits(int i);

void initServo();
void initCubes();
