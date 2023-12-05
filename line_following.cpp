#include "line_following.h"
#include "sensorbar.h"
#include "shift_register.h"
#include "cube_tray.h"

// Stuff for DC Motors
#define BIAS 30
#define TOP_RIGHT_SPEED 255
#define TOP_LEFT_SPEED TOP_RIGHT_SPEED - BIAS
#define TURN_SPEED 150
#define BALANCE_DELTA 50
#define TURN_OVERSHOOT 1000
#define SX1509_ADDRESS 0x3E  // SX1509 I2C address (00)

SensorBar mySensorBar(SX1509_ADDRESS);

int leftSide, rightSide;

bool b[8]; // line following array sensors b0 through b7

// Uses two variables called rightSide and leftSide to determine where the robot should go (turn or move straight and try to balance)
void followLine()
{
  readSensors();
  
  if     (atTurn() == 'R') turn('R'); 
    
  else if(atTurn() == 'L') turn('L');  
    
  else if(b[4] == 0 && b[3] == 1 && b[2] == 0){
    fwd('r', TOP_RIGHT_SPEED-BALANCE_DELTA);
    fwd('l', TOP_LEFT_SPEED);
  }
    
  else if(b[4] == 1 && b[3] == 0 && b[5] == 0){
    fwd('r', TOP_RIGHT_SPEED);
    fwd('l', TOP_LEFT_SPEED-BALANCE_DELTA);
  }
    
  else if(b[4] == 0 && b[3] == 1 && b[2] == 1){
    fwd('r', TOP_RIGHT_SPEED-(BALANCE_DELTA+30));
    fwd('l', TOP_LEFT_SPEED);
  }
    
  else if(b[4] == 1 && b[3] == 0 && b[5] == 1){
    fwd('r', TOP_RIGHT_SPEED);
    fwd('l', TOP_LEFT_SPEED-(BALANCE_DELTA+30));
  }
    
  else if(leftSide != rightSide) balanceWheels(); 

  else if(leftSide == 0 && rightSide == 0) stopWheels(); 

  else{
    fwd('l', TOP_LEFT_SPEED);
    fwd('r', TOP_RIGHT_SPEED);
  }
  
}

void balanceWheels()
{
  int leftWheelSpeed = 40, rightWheelSpeed = 40;
  
  if(leftSide > rightSide){
    fwd('l', 50);
    
    while(b[3] == 0 && b[4] == 0){
      readSensors();
      
      if     (atTurn() == 'R'){ turn('R'); break; }
      else if(atTurn() == 'L'){ turn('L'); break; }
      
      if(leftWheelSpeed >= 255) break;
      
      leftWheelSpeed += 10;
      fwd('l', leftWheelSpeed);
    }
  }
  else if(rightSide > leftSide){
    fwd('r', 50);
    
    while(b[3] == 0 && b[4] == 0){
      readSensors();

      if     (atTurn() == 'R'){ turn('R'); break; }
      else if(atTurn() == 'L'){ turn('L'); break; }

      if(rightWheelSpeed >= 255) break;
      
      rightWheelSpeed += 10;
      fwd('r', rightWheelSpeed);
    }
  }
  
}

char atTurn()
{
  readSensors();
  if     (leftSide  >= 3 && b[7] == 1 && b[6] == 1) return 'L';
  else if(rightSide >= 3 && b[0] == 1 && b[1] == 1) return 'R';
}

void turn(char dir)
{
  //we want the robot to move straight during turn overshoot delay
  fwd('l', TOP_LEFT_SPEED);
  fwd('r', TOP_RIGHT_SPEED);

  delay(TURN_OVERSHOOT); // over shoot a bit so that wheels are inline with turn

  readSensors(); // sets b[0] to b[7] to 0

  if(dir == 'R'){
    rev('r', TURN_SPEED);
    fwd('l', TURN_SPEED);
    
    while(b[3] == OFF && b[4] == OFF){
      readSensors();
    }
  }
    
  else if(dir == 'L'){
    fwd('r', TURN_SPEED-50);
    rev('l', TURN_SPEED+50);
    
    while(b[3] == OFF && b[4] == OFF){
      readSensors();
    }
  }
  
}

void fwd(char motor, int motor_speed)
{   
  if(motor == 'l'){
    analogWrite(MOTOR_L_VCC, motor_speed);
    shiftWrite(MOTOR_L_GND, LOW);
  }
  if(motor == 'r'){
    analogWrite(MOTOR_R_VCC, motor_speed);
    shiftWrite(MOTOR_R_GND, LOW);
  }
}

void rev(char motor, int motor_speed)
{  
  if(motor == 'l'){
    analogWrite(MOTOR_L_VCC, 255 - motor_speed);
    shiftWrite(MOTOR_L_GND, HIGH);
  }
  if(motor == 'r'){
    analogWrite(MOTOR_R_VCC, 255 - motor_speed);
    shiftWrite(MOTOR_R_GND, HIGH);
  }
}

void stopWheels(){
  fwd('l', 0);
  fwd('r', 0);
}

// used to get the ultrasonic sensor away from the bin
void leaveBin(){
  Serial.println("Leaving bin");
  for(int i = 0; i < 7; i++){
    readSensors();
    followLine();
    delay(100);
  }
}

void readSensors()
{ 
  leftSide = 0; rightSide = 0;
  
  uint8_t rawValue = mySensorBar.getRaw(); 
  for(int i = 0; i < 8; i++) {
    b[i] = (rawValue >> i) & 1;
  } 
  
  for(int i = 0; i < 4; i++){
    if(b[i]) rightSide++;
    if(b[i+4]) leftSide++;
  } 
}

void initLineFollowingArray(){
  mySensorBar.clearBarStrobe();
  mySensorBar.clearInvertBits();
  uint8_t returnStatus = mySensorBar.begin();
}

void initMotors(){
  pinMode(MOTOR_R_VCC, OUTPUT);
  pinMode(MOTOR_L_VCC, OUTPUT);
}
