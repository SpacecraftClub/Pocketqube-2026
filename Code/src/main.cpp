#include "compileOptions.h"

#if DEBUG
  #pragma message "Compiling in debug mode"
#endif

#define TEST_TIMING true
#define PRINT_SENSOR_CSV false

#include <Arduino.h>
#include <SdFat.h>
#include <SPI.h>
#include "pinDefinitions.h"
#include "powerMonitor.h"
#include "taskHandles.h"
#include "camera.h"
#include "sensor_logger.h"
#include "radio.h"

#define HAS_TASK_ELLAPSED(ms, lastExecution, period) ((((ms) - (lastExecution)) >= (period)) || ((lastExecution) > (ms)))

extern SENSOR_TASK* sensorTasks[NUM_SENSOR_TASKS];

extern CAMERA_TASK arducamMegaCameraContext;

SdFs SD_CARD;

uint64_t lastSensorRun = 0;
const uint32_t sensorPeriod_ms = 100;
uint64_t lastCSVSave = 0;
const uint32_t CSVSavePeriod = 1000;
uint64_t lastRadioTx = 0;
const uint32_t radioPeriod_ms = 5000;
uint64_t lastPhotoTaken;
const uint32_t photoShootPeriod = 15000;

void setup() {
  #if DEBUG
    Serial.begin(9600);
    delay(5000);
    Serial.println("\n\nStarting QubeSat");
  #endif
  SPI.begin();
  Wire.begin();
  pinMode(CAM_CS, OUTPUT);
  pinMode(RAD_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(CAM_CS,HIGH);
  digitalWrite(RAD_CS, HIGH);
  digitalWrite(SD_CS,HIGH);

  //Initialize SD Card
  #if SD_ENABLE
  if(!SD_CARD.begin(SD_CONFIG)){
    Serial.println("SD Card Failed to initialize!");
  }
  #if FORMAT_SD_CARD
  if(SD_CARD.format(&Serial)){
    Serial.println("Reformatted the SD Card");
  } else {
    Serial.println("Failed to reformat the SD Card");
  }
  #endif
  #endif

  setupSensorTasks();
  runSensorTasks(true); // make sure they are up to speed
  lastCSVSave = millis() + CSVSavePeriod / 2;

  #if CAMERA_ENABLE
  TASK_RETURN_CODE returnCode =  arducamMegaCameraContext.setup(&arducamMegaCameraContext);

  if(returnCode != TASK_EXECUTION_OKAY){
    Serial.print("Failed to initialize Task ");
    Serial.print("[Camera]");
    Serial.print(", failed with error code: ");
    Serial.println(Task_Return_Code_Names[returnCode]);
  }
  #endif

  initializeLogger(sensorTasks, &arducamMegaCameraContext);

  #if RADIO_ENABLE
  TASK_RETURN_CODE radioReturnCode = initRadio(sensorTasks, NUM_SENSOR_TASKS);
  if(radioReturnCode != TASK_EXECUTION_OKAY){
    Serial.print("Failed to initialize Task ");
    Serial.print("[Radio]");
    Serial.print(", failed with error code: ");
    Serial.println(Task_Return_Code_Names[radioReturnCode]);
  } else {
    lastRadioTx = millis() + radioPeriod_ms / 2;
  }
  #endif

  #if DEBUG
  Serial.println("\nQubeSat Initialized\n\n");
  #endif
}

void loop() {
  #if TEST_TIMING
  static uint32_t sensors_WCET = 0;
  static uint32_t camera_WCET = 0;
  #endif

  uint64_t currMillis = millis();
  if(HAS_TASK_ELLAPSED(currMillis, lastCSVSave, CSVSavePeriod)){
    lastCSVSave = currMillis;
    lastSensorRun = currMillis;
    runSensorTasks(true);

    logNewData();

    #if (DEBUG & PRINT_SENSOR_CSV)
    static bool havePrintedLabels = false;
    if(!havePrintedLabels){
      havePrintedLabels = true;
      Serial.println();
      for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
        for(uint8_t data = 0; data < sensorTasks[sensor]->numDataTypes; data++){
          Serial.print(sensorTasks[sensor]->dataNames[data]);
          if(sensor != NUM_SENSOR_TASKS - 1 || data != sensorTasks[sensor]->numDataTypes - 1){
            Serial.print(',');
          }
        }
      }
      Serial.println();
    }
    for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
      for(uint8_t data = 0; data < sensorTasks[sensor]->numDataTypes; data++){
        Serial.print(sensorTasks[sensor]->dataCSV[data]);
        Serial.print(',');
      }
    }
    Serial.println();
    #endif

  } else if(HAS_TASK_ELLAPSED(currMillis, lastSensorRun, sensorPeriod_ms)){
    lastSensorRun = currMillis;
    runSensorTasks(false);
  }
  #if RADIO_ENABLE
  uint64_t radioMillis = millis();
  if(HAS_TASK_ELLAPSED(radioMillis, lastRadioTx, radioPeriod_ms)){
    lastRadioTx = radioMillis;
    TASK_RETURN_CODE radioReturnCode = transmitTelemetry();
    if(radioReturnCode != TASK_EXECUTION_OKAY){
      Serial.print("Failed to tick Task ");
      Serial.print("[Radio]");
      Serial.print(", failed with error code: ");
      Serial.println(Task_Return_Code_Names[radioReturnCode]);
    }
  }
  #endif
  #if TEST_TIMING
  uint32_t sensorExecutionTime = millis() - currMillis;
  if(sensorExecutionTime > sensors_WCET && millis() > currMillis){
    sensors_WCET = sensorExecutionTime;
    Serial.print("New Sensors WCET: ");
    Serial.println(sensors_WCET);
  }
  #endif
  #if CAMERA_ENABLE
  currMillis = millis(); // make sure to update before checking camera since sensor reading can take a while
  if(HAS_TASK_ELLAPSED(currMillis, lastPhotoTaken, photoShootPeriod)){
    lastPhotoTaken = currMillis;
    TASK_RETURN_CODE returnCode = arducamMegaCameraContext.tick(&arducamMegaCameraContext);
    if(returnCode != TASK_EXECUTION_OKAY){
      Serial.print("Failed to tick Task ");
      Serial.print("[Camera]");
      Serial.print(", failed with error code: ");
      Serial.println(Task_Return_Code_Names[returnCode]);
    }
  }
  #endif
  #if TEST_TIMING
  uint32_t cameraExecutionTime = millis() - currMillis;
  if(cameraExecutionTime > camera_WCET && millis() > currMillis){
    camera_WCET = cameraExecutionTime;
    Serial.print("New Camera WCET: ");
    Serial.println(camera_WCET);
  }
  #endif
  
}
