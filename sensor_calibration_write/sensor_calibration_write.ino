#include "Adafruit_Sensor_Calibration.h"

// instructions https://learn.adafruit.com/how-to-fuse-motion-sensor-data-into-ahrs-orientation-euler-quaternions/storing-calibrations

// select either EEPROM or SPI FLASH storage:
#ifdef ADAFRUIT_SENSOR_CALIBRATION_USE_EEPROM
  Adafruit_Sensor_Calibration_EEPROM cal;
#else
  Adafruit_Sensor_Calibration_SDFat cal;
#endif

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  delay(100);
  Serial.println("Calibration filesys test");
  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
    while (1) yield();
  }
  Serial.print("Has EEPROM: "); Serial.println(cal.hasEEPROM());
  Serial.print("Has FLASH: "); Serial.println(cal.hasFLASH());

  if (! cal.loadCalibration()) {
    Serial.println("No calibration loaded/found... will start with defaults");
  } else {
    Serial.println("Loaded existing calibration");
  }

  // in uTesla
  cal.mag_hardiron[0] = -161.68;
  cal.mag_hardiron[1] =  -64.09;
  cal.mag_hardiron[2] =  142.68;

  // in uTesla
  cal.mag_softiron[0] =  1.01;
  cal.mag_softiron[1] =  0.04;
  cal.mag_softiron[2] = -0.02;  
  cal.mag_softiron[3] =  0.04;
  cal.mag_softiron[4] =  1.01;
  cal.mag_softiron[5] = -0.02;  
  cal.mag_softiron[6] = -0.02;
  cal.mag_softiron[7] = -0.02;
  cal.mag_softiron[8] =  0.98;
  
  // Earth total magnetic field strength in uTesla (dependent on location and time of the year),
  // visit: https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#igrfwmm)
  cal.mag_field = 49.37; 

  // in Radians/s
  cal.gyro_zerorate[0] =  0.00;
  cal.gyro_zerorate[1] =  0.00;
  cal.gyro_zerorate[2] =  0.00;

  cal.accel_zerog[0] = 0;
  cal.accel_zerog[1] = 0;
  cal.accel_zerog[2] = 0;
  
  if (! cal.saveCalibration()) {
    Serial.println("**WARNING** Couldn't save calibration");
  } else {
    Serial.println("Wrote calibration");    
  }

  cal.printSavedCalibration();
}

void loop() {

}
