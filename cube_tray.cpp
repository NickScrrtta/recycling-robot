#include "cube_tray.h"
#include "line_following.h"
#include "colour_sensor.h"
#include <Stepper.h>
#include <Servo.h>

// Stuff for Stepper Motor
#define STEPS 32
#define STEPPER_SPEED 500
#define STEPPER_STEPS 513

#define HALL_EFFECT_UPPER_BOUND    600
#define HALL_EFFECT_LOWER_BOUND    510

#define PHOTO_RESISTOR_MIN         500

Stepper stepperMotor(STEPS, ORANGE, PINK, YELLOW, BLUE);
Servo servo;

int cube_at_door = 0;
byte cubes[4]; 

/* bits 2 and 3 determine the type of cube:
 * 
 * 00xx -> plastic cube -> red bin
 * 01xx -> magnetic cube -> white bin
 * 10xx -> metal cube -> green bin
 * 11xx -> clear cube -> blue bin
 * 
 * bits 0 and 1 determine the position of the cube in the tray:
 * 
 * xx00 -> at the door
 * xx01 -> one turn cw from the door
 * xx10 -> opposite side of the door
 * xx11 -> one turn ccw from the door
 * 
 * bit 4 corresponds to whether or not we have the cube
 * 0 -> we don't have it. 1 -> we do have it
 */

void takeInventory()
{
  bitSet(cubes[1], 0);
  bitSet(cubes[2], 1);
  bitSet(cubes[3], 0);
  bitSet(cubes[4], 1);
   
  for(int j = 0; j < 4; j++){
    Serial.print("Rotation ");
    Serial.println(j);
    
    delay(800);  
    
    if(checkMagnetic()){
      if(j == 3){
        bitSet(cubes[0], 2);
        bitSet(cubes[0], 4);
      }
      else{
        bitSet(cubes[j+1], 2);
        bitSet(cubes[j+1], 4);
      }
    } 
    
    if(checkMetal()){
      if(j < 2){
        bitSet(cubes[j+2], 3);
        bitSet(cubes[j+2], 4);
      }
      else{
        bitSet(cubes[j-2], 3);
        bitSet(cubes[j-2], 4);
      }
    } 
    
    if(checkClear()){
      if(j == 0){
        bitSet(cubes[3], 2);
        bitSet(cubes[3], 3);
        bitSet(cubes[3], 4);
      }
      else{
        bitSet(cubes[j-1], 2);
        bitSet(cubes[j-1], 3);
        bitSet(cubes[j-1], 4);
      }
    }
    
    Serial.println(" "); 

    for(int i = 0; i < 4; i++){
      printCubeBits(i);
    }
    
    Serial.println(" ");
    rotate(90);
  }

  for(int k = 0; k < 4; k++){
    if(bitRead(cubes[k], 2) == 0 && bitRead(cubes[k], 3) == 0) bitSet(cubes[k], 4);
  }

  printCubes();
}

void dispenseCube()
{
  stopWheels();
  delay(200);
  
  char colour = checkColour();
  int cube_to_dispense;

  printColours();
  
  if     (colour == 'r') cube_to_dispense = findCube(0,0);
  else if(colour == 'w') cube_to_dispense = findCube(0,1);
  else if(colour == 'g') cube_to_dispense = findCube(1,0);
  else if(colour == 'b') cube_to_dispense = findCube(1,1);
  else cube_to_dispense = -1;

  int rotations_needed = ( (4-cube_at_door) + cube_to_dispense )%4;
  
  // cube_to_dispense = -1 if findCube cannot find the cube or if checkColour returns 'e'
  if(cube_to_dispense >= 0){ 
    rotate(90 *  rotations_needed ); 
    pushCube(); 
    Serial.println("Cube has been pushed");
    bitClear(cubes[cube_to_dispense], 4); 
  } 

  leaveBin(); 
}

bool checkMagnetic()
{  
  int hallEffectValue = analogRead(HALL_EFFECT);
 
  if(hallEffectValue < HALL_EFFECT_LOWER_BOUND || hallEffectValue > HALL_EFFECT_UPPER_BOUND){
    Serial.print("checkMagnetic PASSED, hallEffectValue = ");
    Serial.println(hallEffectValue);
    return TRUE;
  }
  else{
    Serial.print("checkMagnetic FAILED, hallEffectValue = ");
    Serial.println(hallEffectValue);
    return FALSE;
  }
}

bool checkMetal()
{ 
  if(digitalRead(METAL_DETECTOR) == 0) Serial.println("checkMetal PASSED");
  else Serial.println("checkMetal FAILED");
  
  return (!digitalRead(METAL_DETECTOR)); 
}

bool checkClear()
{
  int photoResistorValue = analogRead(PHOTO_RESISTOR);

  if (photoResistorValue > PHOTO_RESISTOR_MIN){
    Serial.print("checkClear PASSED, photoResistorValue = ");
    Serial.println(photoResistorValue);
    return TRUE; 
  }
  else{
    Serial.print("checkClear FAILED, photoResistorValue = ");
    Serial.println(photoResistorValue);
    return FALSE;
  }
}

void printCubes()
{
  for(int i = 0; i < 4; i++){
    if     (bitRead(cubes[i], 3) == 0 && bitRead(cubes[i], 2) == 0){ Serial.print("Cube in position "); Serial.print(i); Serial.print(" is PLASTIC   |   "); printCubeBits(i); }
    else if(bitRead(cubes[i], 3) == 0 && bitRead(cubes[i], 2) == 1){ Serial.print("Cube in position "); Serial.print(i); Serial.print(" is MAGNETIC  |   "); printCubeBits(i); }
    else if(bitRead(cubes[i], 3) == 1 && bitRead(cubes[i], 2) == 0){ Serial.print("Cube in position "); Serial.print(i); Serial.print(" is METAL     |   "); printCubeBits(i); }
    else if(bitRead(cubes[i], 3) == 1 && bitRead(cubes[i], 2) == 1){ Serial.print("Cube in position "); Serial.print(i); Serial.print(" is CLEAR     |   "); printCubeBits(i); }
  }
}

void pushCube(){
  servo.write(50); delay(100);
  servo.write(10); delay(200);
  servo.write(50); delay(100);
}

int findCube(int bit3, int bit2){
  for(int i = 0; i < 4; i++){
    if(bitRead(cubes[i], 3) == bit3 && bitRead(cubes[i], 2) == bit2 && bitRead(cubes[i], 4) == 1){
      return i;
    }
  }
  
  return -1;
}

// rotates the platform 0/90/180/270 degrees counterclockwise
void rotate(int deg){
  for(int i = 0; i < (deg/90); i++){
    stepperMotor.setSpeed(STEPPER_SPEED);
    stepperMotor.step(-STEPPER_STEPS);
    cube_at_door++;
  }
  cube_at_door = cube_at_door%4;
}

void printCubeBits(int i){
  for(int j = 7; j >= 0; j--){
    Serial.print(bitRead(cubes[i], j));
  }
  Serial.println(" ");
}

// move door to starting position
void initServo(){
  servo.attach(SERVO_PIN);
  servo.write(50);
} 

void initCubes(){
  for(int i = 0; i < 4; i++){
    for(int j = 0; j < 8; j++){
      bitClear(cubes[i], j);
    }
  }
}
