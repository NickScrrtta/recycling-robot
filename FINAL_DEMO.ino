#include "colour_sensor.h"
#include "pinout.h"
#include "shift_register.h"
#include "cube_tray.h"
#include "line_following.h"
#include "sonar_sensor.h"

void setup() {

  initShiftRegister(); // initialize shift register  
  initMotors();
  initUltrasonic(); 
  initServo(); // put servo in starting positions
  initLineFollowingArray();
  initCubes(); // set all bits of all cubes to 0
  
  Serial.begin(9600); // Starts the serial communication
  
  delay(3000);
  takeInventory(); // determine what cubes we have on tray
}

void loop()
{ 
  everything();
}

void everything()
{
  followLine();

  if(binDetected()) dispenseCube();
}
