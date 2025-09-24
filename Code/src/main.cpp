#define DEBUG true

#define POWER_MONITOR false
#define SD_ENABLE true
#define CAMERA_ENABLE true
#define OLD_BOARD false
#define I2C_SENSORS true
#define RADIO_ENABLE false

#if DEBUG
  #pragma message "Compiling in debug mode"
#endif

#include <Arduino.h>
#include <SD.h>
#include "pinDefinitions.h"
#include "camera.h"
#include "dataLogger.h"
#include "sensors.h"
#include "powerMonitor.h"
#include "radio.cpp"

CameraManager camera;

DataPersistance datalogger;

MMCModule magneticSensor;

LMSModule gyroAccelSensor;

MS5611Module atmosphericSensor;

PowerMonitor powerMonitor;

#if RADIO_ENABLE
RadioManager radioManager;
#endif

int lastPhotoTaken = 0;

#define CAMERA_REFRESH_DELAY 10000
unsigned long lastCameraRefresh = 0;

#define CSV_UPATE_DELAY 10000
unsigned long lastCSVUpdate = 0;

#define SENSOR_REFRESH_DELAY 1000
unsigned long lastSensorRefresh = 0;

void updateSensors();
void updateCamera();

void setup() {

  #if DEBUG
    Serial.begin(9600);
    Serial.println("\n\nStarting QubeSat");
  #endif
  pinMode(CAM_CS, OUTPUT);
  pinMode(RAD_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(CAM_CS,HIGH);
  digitalWrite(RAD_CS, HIGH);
  digitalWrite(SD_CS,HIGH);

  //Initialize SD Card
  #if SD_ENABLE
  while(!SD.begin(SD_CS)){
    Serial.println("SD Card Error!");
    delay(1000);
  }

  datalogger.init();
  #endif

  #if I2C_SENSORS

  // magneticSensor.init_mag();
  
  gyroAccelSensor.init_LSM6DOX();

  atmosphericSensor.init_MS5611();

  #endif

  #if POWER_MONITOR
  powerMonitor.initSensors();
  #endif

  #if CAMERA_ENABLE
  camera.setup(CAM_CS);
  #endif

  #if RADIO_ENABLE
  radioManager.radio_init();
  #endif

  #if DEBUG
    Serial.println("\nQubeSat Initialized\n\n");
  #endif
}

void loop() {

  if(millis() - lastSensorRefresh >= SENSOR_REFRESH_DELAY || !lastSensorRefresh || millis() < lastSensorRefresh) {
    #if DEBUG
      Serial.println("Refreshing Sensors");
    #endif
    lastSensorRefresh = millis();
    updateSensors();
  }

  #if RADIO_ENABLE
  radioManager.radio_loop();
  #endif

  #if CAMERA_ENABLE
  if(millis() - lastCameraRefresh >= CAMERA_REFRESH_DELAY || millis() < lastCameraRefresh) {
    #if DEBUG
      Serial.println("Capturing Photo");
    #endif
    lastCameraRefresh = millis();
    lastPhotoTaken = camera.takePicture();
  }
  #endif

  if(millis() - lastCSVUpdate >= CSV_UPATE_DELAY || millis() < lastCSVUpdate){
    #if DEBUG
      Serial.println("Updating CSV");
    #endif
    lastCSVUpdate = millis();
    datalogger.addToCSV(lastPhotoTaken);
    if(lastPhotoTaken)
      lastPhotoTaken = 0;
  }

}

void updateSensors(){
  static double dataIn[NUM_ITEMS_IN_CSV];
  static bool firstTime = true;
  if(firstTime){
    for(int i = 0; i < NUM_ITEMS_IN_CSV; i++){
      dataIn[i] = 0.0;
    }
    firstTime = false;
  }

  #if I2C_SENSORS
  // magneticSensor.tick();
  // float* magneticData = magneticSensor.get_data();
  // dataIn[MAGNETIC_X] = magneticData[0];
  // dataIn[MAGNETIC_Y] = magneticData[1];
  // dataIn[MAGNETIC_Z] = magneticData[2];
  // dataIn[MAGNETIC_DEGREES] = magneticData[3];

  gyroAccelSensor.tick();
  float* gyroData = gyroAccelSensor.get_data();
  dataIn[ACCELERATION_X] = gyroData[0];
  dataIn[ACCELERATION_Y] = gyroData[1];
  dataIn[ACCELERATION_Z] = gyroData[2];
  dataIn[GYRO_X] = gyroData[3];
  dataIn[GYRO_Y] = gyroData[4];
  dataIn[GYRO_Z] = gyroData[5];
  dataIn[IMU_TEMP] = gyroData[6];

  atmosphericSensor.tick();
  float* atmosphericData = atmosphericSensor.get_data();
  dataIn[PRESSURE] = atmosphericData[0];
  dataIn[ATMOSPHERE_TEMP] = atmosphericData[1];
  #endif

  #if POWER_MONITOR
  dataIn[SOLAR_CURRENT] = powerMonitor.readCurrent(SOLAR_SENSOR);
  dataIn[SOLAR_VOLTS] = powerMonitor.readVoltage(SOLAR_SENSOR);
  dataIn[SOLAR_POWER] = powerMonitor.readPower(SOLAR_SENSOR);
  dataIn[BATTERY_CURRENT] = powerMonitor.readCurrent(BATTERY_SENSOR);
  dataIn[BATTERY_VOLTS] = powerMonitor.readVoltage(BATTERY_SENSOR);
  dataIn[BATTERY_POWER] =  powerMonitor.readPower(BATTERY_SENSOR);
  #endif
  
  #if SD_ENABLE
  datalogger.addData(dataIn);
  #else
  for(int i = 0; i < NUM_ITEMS_IN_CSV; i++){
    Serial.print(dataIn[i]);
    Serial.print(',');
    Serial.println();
  }
  #endif
}