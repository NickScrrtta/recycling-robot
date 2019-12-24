#include <Arduino.h>
#include "pinout.h"

void readSensors();
void followLine();
void balanceWheels();
char atTurn();
void turn(char dir);

void fwd(char motor, int motor_speed);
void rev(char motor, int motor_speed);
void stopWheels();

bool binDetected();
void leaveBin();

void initLineFollowingArray();
void initMotors();
void initUltrasonic();
