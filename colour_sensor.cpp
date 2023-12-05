#include "colour_sensor.h"

char colour = 'e';

int red_f = 0, green_f = 0, blue_f = 0, white_f = 0;

char checkColour(){

  shiftWrite(S2, LOW);  shiftWrite(S3, LOW);  red_f   = pulseIn(COLOUR_SENSOR, LOW);
  shiftWrite(S2, HIGH); shiftWrite(S3, HIGH); green_f = pulseIn(COLOUR_SENSOR, LOW);
  shiftWrite(S2, LOW);  shiftWrite(S3, HIGH); blue_f  = pulseIn(COLOUR_SENSOR, LOW);
  shiftWrite(S2, HIGH); shiftWrite(S3, LOW);  white_f = pulseIn(COLOUR_SENSOR, LOW);

  // White detected
  if(white_f < 400 && red_f < 1200){ 
    for(int i = 0; i < 3; i++){
      turnOnLEDs();  delay(200);
      turnOffLEDs(); delay(200);
    }
    delay(1000);
    colour = 'w';
  }

  // Green detected
  else if((green_f < blue_f && green_f > red_f) || (green_f < red_f && green_f < blue_f) || (green_f == red_f && green_f < blue_f)){ 
    shiftWrite(GREEN_LED, HIGH);
    delay(1000);
    colour = 'g';
  }

  // Blue detected
  else if(blue_f < green_f && blue_f < red_f){ 
    shiftWrite(BLUE_LED, HIGH);
    delay(1000);
    colour = 'b';
  }

  // Red detected
  else if(red_f < green_f && red_f < blue_f){ 
    shiftWrite(RED_LED, HIGH);
    delay(1000);
    colour = 'r';
  }

  // No colour detected
  else{ 
    for(int i = 0; i < 3; i++){
      shiftWrite(RED_LED, HIGH);   delay(100);
      shiftWrite(GREEN_LED, HIGH); delay(100);
      shiftWrite(BLUE_LED, HIGH);  delay(100);
      turnOffLEDs();               delay(100);
    }
    colour = 'e';
  } 
  
  turnOffLEDs();
  return colour;
}

void turnOnLEDs(){
  shiftWrite(RED_LED, HIGH);
  shiftWrite(GREEN_LED, HIGH);
  shiftWrite(BLUE_LED, HIGH);
}

void turnOffLEDs(){
  shiftWrite(RED_LED, LOW);
  shiftWrite(GREEN_LED, LOW);
  shiftWrite(BLUE_LED, LOW);
}

void printColours()
{
  Serial.print("R = "); Serial.print(red_f);   Serial.print("   |   ");
  Serial.print("G = "); Serial.print(green_f); Serial.print("   |   ");
  Serial.print("B = "); Serial.print(blue_f);  Serial.print("   |   ");
  Serial.print("W = "); Serial.print(white_f); Serial.print("   |   ");
  Serial.print("Colour is "); Serial.println(colour);
}
