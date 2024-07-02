#include "RC.h"
#include "wifi.h"

float dt;
unsigned long current_time, prev_time;

void setup() {
  Serial.begin(115200);
  Serial7.begin(115200);  // Serial7 to communicate with ESP-01

  #if defined CRSF
  crsf.begin();
  crsf.onPacketChannels = &packetChannels;
  #endif

  #if defined PPM
  pinMode(1, INPUT);
  attachInterrupt(1, read_PPM, RISING);
  #endif

    //Initialize IMU communication
  IMUinit();
  delay(5);
}

void loop() {
  prev_time = current_time;      
  current_time = micros();      
  dt = (current_time - prev_time)/1000000.0;

  getIMUdata(); //Pulls raw gyro, accelerometer, and magnetometer data from IMU and LP filters to remove noise
  Madgwick(GyroX, -GyroY, -GyroZ, -AccX, AccY, AccZ, MagY, -MagX, MagZ, dt); //Updates roll_IMU, pitch_IMU, and yaw_IMU angle estimates (degrees)

  // wifi pid values
  if (Serial7.available()) {
    String receivedData = Serial7.readStringUntil('\n');
    processReceivedData(receivedData);
  }

  #if defined CRSF
  crsf.loop();
  #endif
}