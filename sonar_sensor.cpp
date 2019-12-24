#include "sonar_sensor.h"
#include "shift_register.h"

float duration, distance;

bool binDetected()
{
  shiftWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  shiftWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  shiftWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration/2)*0.0343;

//  Serial.print("duration = "); Serial.println(duration);
//  Serial.print("distance = "); Serial.println(distance);

  if(distance <= 5 && distance >= 2) return TRUE;

  return FALSE;
}

void initUltrasonic(){ pinMode(ECHO_PIN, INPUT); }
