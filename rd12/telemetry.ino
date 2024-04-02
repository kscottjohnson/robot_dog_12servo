// Gathers data about the robot

void updateFilter(){
  ahrsFilter.update(gyro_x, gyro_y, gyro_z, 
                accel_x, accel_y, accel_z, 
                mag_x, mag_y, mag_z);
  ahrs_r = ahrsFilter.getRoll();
  ahrs_p = ahrsFilter.getPitch() * -1;
  ahrs_y = ahrsFilter.getYaw();
  
  
  Serial.print("AHRS ");
  Serial.print(ahrs_r);Serial.print(" ");
  Serial.print(ahrs_p);Serial.print(" ");
  Serial.println(ahrs_y);
  
}

void getIMUdata() {
  imu.getEvent(&accel, &gyro, &temp);
  mdl.getEvent(&mag);
  cal.calibrate(accel);
  cal.calibrate(gyro);
  cal.calibrate(mag);
  accel_x = accel.acceleration.x;
  accel_y = accel.acceleration.y;
  accel_z = accel.acceleration.z;
  gyro_x = gyro.gyro.x * SENSORS_RADS_TO_DPS;
  gyro_y = gyro.gyro.y * SENSORS_RADS_TO_DPS;
  gyro_z = gyro.gyro.z * SENSORS_RADS_TO_DPS;
  mag_x = mag.magnetic.x;
  mag_y = mag.magnetic.y;
  mag_z = mag.magnetic.z;

/*
  Serial.print("Accel: ");
  Serial.print(accel_x);Serial.print(" ");
  Serial.print(accel_y);Serial.print(" ");
  Serial.print(accel_z);
  Serial.println(" m/s^2");
  Serial.print("Gyro: ");
  Serial.print(gyro_x);Serial.print(" ");
  Serial.print(gyro_y);Serial.print(" ");
  Serial.println(gyro_z);
*/
}

void setupIMU() {
  // load calibration
  if (!cal.begin()) {
    Serial.println("Failed to initialize calibration helper");
  }
  
  if (! cal.loadCalibration()) {
    Serial.println("**WARNING** No calibration loaded/found");
  }
  cal.printSavedCalibration();

  Serial.println("Calibrations found: ");
  Serial.print("\tMagnetic Hard Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.mag_hardiron[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();
  
  Serial.print("\tMagnetic Soft Offset: ");
  for (int i=0; i<9; i++) {
    Serial.print(cal.mag_softiron[i]); 
    if (i != 8) Serial.print(", ");
  }
  Serial.println();

  Serial.print("\tMagnetic Field Magnitude: ");
  Serial.println(cal.mag_field);

  Serial.print("\tGyro Zero Rate Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.gyro_zerorate[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();

  Serial.print("\tAccel Zero G Offset: ");
  for (int i=0; i<3; i++) {
    Serial.print(cal.accel_zerog[i]); 
    if (i != 2) Serial.print(", ");
  }
  Serial.println();

  if(!imu.begin_I2C()){
    Serial.println("Failed to init IMU");
  }

  imu.configInt1(false, false, true); // accelerometer DRDY on INT1
  imu.configInt2(false, true, false); // gyro DRDY on INT2

  imu.setAccelRange(LSM6DS_ACCEL_RANGE_2_G);
  imu.setGyroRange(LSM6DS_GYRO_RANGE_250_DPS);
  imu.setAccelDataRate(LSM6DS_RATE_104_HZ);
  imu.setGyroDataRate(LSM6DS_RATE_104_HZ);

  if(!mdl.begin_I2C()){
    Serial.println("Failed to init MDL");
  }
  
  mdl.setIntThreshold(500);
  mdl.configInterrupt(false, false, true, true, false, true); 

  mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
  mdl.setDataRate(LIS3MDL_DATARATE_1000_HZ);
  mdl.setPerformanceMode(LIS3MDL_MEDIUMMODE);
  mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
}
