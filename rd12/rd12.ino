#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <bluefruit.h>
#include <Adafruit_LSM6DS3TRC.h> // may need Adafruit_LSM6DS33.h for older boards
#include <Adafruit_LIS3MDL.h>
#include <Adafruit_Sensor_Calibration.h>
#include <Adafruit_AHRS.h>
#include <PID_v1.h>

#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define BACK_LEFT 2
#define BACK_RIGHT 3
#define FEMUR 66
#define CHASSIS_L 127.5
#define CHASSIS_W 83.5
#define LEG_OFFSET 20.5

#include "LegServo.h"

// Servos
Adafruit_PWMServoDriver driverL = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver driverR = Adafruit_PWMServoDriver(0x41);

// Petoi P1S Servos: 270 degrees pwm 100 to 505

LegServo servos[12] = { 
  LegServo(&driverL, 4, 302,  90, false), // Front Left Hip - 0 degrees is down, 300
  LegServo(&driverL, 5, 230,  90, false), // Front Left Shoulder - 0 degrees is forward 235
  LegServo(&driverL, 6, 170,  14, false), // Front Left Knee - 0 degrees is up 175

  LegServo(&driverR, 4, 304,  90, false), // Front Right Hip - 0 degrees is up 300
  LegServo(&driverR, 5, 356,  90,  true), // Front Right Shoulder - 0 degrees is back 370
  LegServo(&driverR, 6, 426,  14,  true), // Front Right Knee - 0 degrees is down 430

  LegServo(&driverL, 0, 300,  90,  true), // Back Left Hip 305
  LegServo(&driverL, 1, 233,  90, false), // Back Left Shoulder 
  LegServo(&driverL, 2, 180,  14, false), // Back Left Knee 

  LegServo(&driverR, 0, 292,  90,  true), // Back Right Hip 
  LegServo(&driverR, 1, 362,  90,  true), // Back Right Shoulder 
  LegServo(&driverR, 2, 424,  14,  true)  // Back Right Knee 
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

// IMU
Adafruit_Sensor_Calibration_SDFat cal;
Adafruit_LSM6DS3TRC imu;
Adafruit_LIS3MDL mdl;
sensors_event_t accel;
sensors_event_t gyro;
sensors_event_t temp;
sensors_event_t mag; 
float accel_x, accel_y, accel_z;
float gyro_x, gyro_y, gyro_z;
float mag_x, mag_y, mag_z;

// filter
Adafruit_NXPSensorFusion ahrsFilter;
#define FILTER_UPDATE_RATE_HZ 100
float ahrs_r, ahrs_p, ahrs_y;
/*
// PID roll adjustment
const double pidRoll_p = 0.2, pidRoll_i = 0, pidRoll_d = 0;
double pidRoll_in, pidRoll_out, pidRoll_set;
float rollAdj = 0;
PID pidRoll(&pidRoll_in, &pidRoll_out, &pidRoll_set, pidRoll_p, pidRoll_i, pidRoll_d, DIRECT);
*/
// PID pitch adjustment
const double pidPitch_p = 0.6, pidPitch_i = 0.002, pidPitch_d = 0.012;
double pidPitch_in, pidPitch_out, pidPitch_set;
float pitchAdj = 0;
PID pidPitch(&pidPitch_in, &pidPitch_out, &pidPitch_set, pidPitch_p, pidPitch_i, pidPitch_d, DIRECT);


// Timing
#define CLOCK_CYCLE 10
unsigned long currentMs;
unsigned long prevMs;
unsigned long connectedMs;
unsigned long filterMs;

void setup() {
  Serial.begin(115200);
  //while (!Serial) yield();
  delay(500);
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

  setupIMU();
  getIMUdata();
  ahrsFilter.begin(FILTER_UPDATE_RATE_HZ);
  updateFilter();

  setupPID();

  prevMs = millis();
  filterMs = prevMs;
}

void loop() {
  currentMs = millis();
  if((currentMs - prevMs) < CLOCK_CYCLE) return;
  prevMs = currentMs;

  if(! Bluefruit.Periph.connected()){
    Serial.println("Not Connected");
    //stand();
    //centerServos();
    delay(1000);
    return;
  }
  if((currentMs - connectedMs) < 1000){ // wait a second after connecting
    return;
  }

  getIMUdata();
  
  if((currentMs - filterMs) > (1000 / FILTER_UPDATE_RATE_HZ)){
    updateFilter();
    filterMs = currentMs;
  }
  

  //centerServos();
  //manualLeg();
  //balanceTest();
  ahrsBalance();

  /*
  if(mode == 0) stand();
  else if(mode == 1) demo();
  else if(mode == 2) staticWalk();
  else stand();
  */

}
