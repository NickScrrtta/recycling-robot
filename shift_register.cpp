#include <Arduino.h>
#include "shift_register.h"

byte currentShiftState;

void shiftRegister()
{ 
  digitalWrite(LATCH_PIN, LOW); 
  digitalWrite(CLOCK_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, currentShiftState); 
  digitalWrite(LATCH_PIN, HIGH); 
}

void shiftWrite(int shiftRegPin, int value)
{ 
  bitWrite(currentShiftState, shiftRegPin, value); 
  shiftRegister(); 
}

void initShiftRegister()
{
  pinMode(DATA_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT); 
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);

  digitalWrite(CLOCK_PIN, LOW);
  digitalWrite(LATCH_PIN, LOW);

  currentShiftState = 0;
  shiftRegister(); // set all outputs of shift register to 0
  digitalWrite(OE_PIN, LOW);
}
