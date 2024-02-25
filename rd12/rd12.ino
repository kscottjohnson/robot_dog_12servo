#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define BACK_LEFT 2
#define BACK_RIGHT 3
#define FEMUR 120

#include "LegServo.h"

// Servos
Adafruit_PWMServoDriver driverL = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver driverR = Adafruit_PWMServoDriver(0x41);

// JX CLS6336HV Servo pwm range: 100 to 586 (2.7 / degree)

LegServo servos[12] = { 
  LegServo(&driverL, 0, 343,  90), // Front Left Hip - 0 degrees is down, 343
  LegServo(&driverL, 1, 142,  90), // Front Left Shoulder - 0 degrees is forward
  LegServo(&driverL, 2, 172,  12), // Front Left Knee - 0 degrees is up 164
  LegServo(&driverR, 0, 346,  90), // Front Right Hip - 0 degrees is up
  LegServo(&driverR, 1, 518,  90), // Front Right Shoulder - 0 degrees is back
  LegServo(&driverR, 2, 515, 168), // Front Right Knee - 0 degrees is down
  LegServo(&driverL, 4, 355,  90), // Back Left Hip 
  LegServo(&driverL, 5, 152,  90), // Back Left Shoulder 
  LegServo(&driverL, 6, 168,  12), // Back Left Knee 
  LegServo(&driverR, 4, 337,  90), // Back Right Hip 
  LegServo(&driverR, 5, 542,  90), // Back RightShoulder 
  LegServo(&driverR, 6, 521, 168)  // Back Right Knee 
};


void setup() {
  Serial.begin(115200);
  //while (!Serial) yield();
  delay(200);
  Serial.println("Robot Dog - 12 Servo");

  // start servos
  driverL.begin();
  driverL.setOscillatorFrequency(25000000);
  driverL.setPWMFreq(50);

  delay(100);

  driverR.begin();
  driverR.setOscillatorFrequency(25000000);
  driverR.setPWMFreq(50);

  delay(100);
}

void loop() {
  //Serial.println("here");
  for(int s=0; s<12; s++){
    servos[s].setDefault();
    delay(10);
  } 
  delay(1000);


}
