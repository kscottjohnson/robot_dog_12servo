#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <bluefruit.h>

#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define BACK_LEFT 2
#define BACK_RIGHT 3
#define FEMUR 120
#define LEG_OFFSET 22

#include "LegServo.h"

// Servos
Adafruit_PWMServoDriver driverL = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver driverR = Adafruit_PWMServoDriver(0x41);

// JX CLS6336HV Servo pwm range: 100 to 586 (2.7 / degree)
// Servo reversal is in relation to the front left leg

LegServo servos[12] = { 
  LegServo(&driverL, 0, 343,  90, false), // Front Left Hip - 0 degrees is down, 343
  LegServo(&driverL, 1, 142,  90, false), // Front Left Shoulder - 0 degrees is forward
  LegServo(&driverL, 2, 172,  12, false), // Front Left Knee - 0 degrees is up 164
  LegServo(&driverR, 0, 346,  90, false), // Front Right Hip - 0 degrees is up
  LegServo(&driverR, 1, 518,  90,  true), // Front Right Shoulder - 0 degrees is back
  LegServo(&driverR, 2, 515,  12,  true), // Front Right Knee - 0 degrees is down
  LegServo(&driverL, 4, 355,  90,  true), // Back Left Hip 
  LegServo(&driverL, 5, 152,  90, false), // Back Left Shoulder 
  LegServo(&driverL, 6, 168,  12, false), // Back Left Knee 
  LegServo(&driverR, 4, 337,  90,  true), // Back Right Hip 
  LegServo(&driverR, 5, 542,  90,  true), // Back RightShoulder 
  LegServo(&driverR, 6, 521,  12,  true)  // Back Right Knee 
};


Leg legs[4] = {
  Leg(&servos[ 0], &servos[ 1], &servos[ 2]), // Front Left
  Leg(&servos[ 3], &servos[ 4], &servos[ 5]), // Front Right
  Leg(&servos[ 6], &servos[ 7], &servos[ 8]), // Back Left
  Leg(&servos[ 9], &servos[10], &servos[11]), // Back Right
};

// Bluetooth
BLEUart bleuart;
uint8_t controlMsg[6] = {128,128,128,128,0,0};
uint8_t rx, ry, lx, ly;
bool buttonAR, buttonBR, buttonCR, buttonR, buttonL;
uint8_t mode = 0;

#define CLOCK_CYCLE 100
unsigned long currentMs;
unsigned long prevMs;
unsigned long connectedMs;


void setup() {
  Serial.begin(115200);
  //while (!Serial) yield();
  delay(200);
  Serial.println("Robot Dog - 12 Servo");

  startBleAdv();

  delay(200);

  // start servos
  driverL.begin();
  driverL.setOscillatorFrequency(25000000);
  driverL.setPWMFreq(50);

  delay(100);

  driverR.begin();
  driverR.setOscillatorFrequency(25000000);
  driverR.setPWMFreq(50);

  delay(100);

  prevMs = millis();
}

void loop() {
  currentMs = millis();
  if((currentMs - prevMs) < CLOCK_CYCLE) return;
  prevMs = currentMs;

  if(! Bluefruit.Periph.connected()){
    Serial.println("Not Connected");
    stand();
    delay(1000);
    return;
  }
  if((currentMs - connectedMs) < 1000){ // wait a second after connecting
    return;
  }
  
  //centerServos();
  
  if(mode == 0) stand();
  else if(mode == 1) stand();
  else if(mode == 2) stand();
  else stand();


}
