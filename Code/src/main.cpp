#define DEBUG true

#define POWER_MONITOR false
#define SD_ENABLE false
#define CAMERA_ENABLE false
#define OLD_BOARD false
#define I2C_SENSORS true
#define RADIO_ENABLE false

#if DEBUG
  #pragma message "Compiling in debug mode"
#endif

#include <Arduino.h>
#include <SD.h>
#include "pinDefinitions.h"
#include "dataLogger.h"
#include "powerMonitor.h"
#include "taskHandles.h"

#define HAS_TASK_ELLAPSED(ms, lastExecution, period) ((((ms) - (lastExecution)) >= (period)) || ((lastExecution) > (ms)))

extern SENSOR_TASK* sensorTasks[NUM_SENSOR_TASKS];

uint64_t lastSensorRun = 0;
const uint32_t sensorPeriod_ms = 100;
uint64_t lastCSVSave = 0;
const uint32_t CSVSavePeriod = 1000;
uint64_t lastPhotoTaken;
const uint32_t photoShootPeriod = 5000;

void setup() {

  #if DEBUG
    Serial.begin(9600);
    delay(5000);
    Serial.println("\n\nStarting QubeSat");
  #endif
  Wire.begin();
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

  setupSensorTasks();

  #if DEBUG
  Serial.println("\nQubeSat Initialized\n\n");
  #endif
}

void loop() {
  uint64_t currMillis = millis();
  if(HAS_TASK_ELLAPSED(currMillis, lastCSVSave, CSVSavePeriod)){
    lastCSVSave = currMillis;
    lastSensorRun = currMillis;
    runSensorTasks(true);

    #if DEBUG
    for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
      for(uint8_t data = 0; data < sensorTasks[sensor]->numDataTypes; data++){
        Serial.print(sensorTasks[sensor]->dataCSV[data]);
        if(data != sensorTasks[sensor]->numDataTypes - 1){
          Serial.print(',');
        }
      }
    }
    Serial.println();
    #endif

    //TODO: run datalogger
  } else if(HAS_TASK_ELLAPSED(currMillis, lastSensorRun, sensorPeriod_ms)){
    lastSensorRun = currMillis;
    runSensorTasks(false);
  }
  currMillis = millis(); // make sure to update before checking camera since sensor reading can take a while
  if(HAS_TASK_ELLAPSED(currMillis, lastPhotoTaken, photoShootPeriod)){
    lastPhotoTaken = currMillis;

  }
  
}