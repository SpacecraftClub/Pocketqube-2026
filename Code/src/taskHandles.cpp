#include "taskHandles.h"
#include "MS5611.h"
#include "MMC5603_sensor.h"
#include <Arduino.h>

const char* Task_Return_Code_Names[] = {
    "TASK_OKAY",
    "INVALID_TASK_CONTEXT",
    "TASK_INCOMPLETE",
    "TASK_INTERRUPTED",
    "TASK_ERROR_HW",
    "TASK_BUS_BUSY",
    "TASK_EXECUTION_OVERTICKED",
    "TASK_STATS_SUBSYS_ERROR",
    "TASK_ERROR_UNSPECIFIED"
};


extern SENSOR_TASK MS5611_Task;

extern SENSOR_TASK MMC5603_Task;

SENSOR_TASK* sensorTasks[NUM_SENSOR_TASKS] = {
    &MS5611_Task,
    &MMC5603_Task
};

void runSensorTask(SENSOR_TASK* taskPtr, bool exportForCSV){
    TASK_RETURN_CODE returnCode = taskPtr->tick(taskPtr, exportForCSV);
    if(returnCode != TASK_EXECUTION_OKAY){
        Serial.print(taskPtr->numDataSamples);
        Serial.print("Sensor Task ");
        Serial.print(taskPtr->taskName);
        Serial.print(" failed with error code: ");
        Serial.println(Task_Return_Code_Names[returnCode]);
    }
}


void runSensorTasks(bool exportForCSV){
    for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
        runSensorTask(sensorTasks[sensor], exportForCSV);
    }
}


void setupSensorTasks(){
    for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
        TASK_RETURN_CODE returnCode = sensorTasks[sensor]->setup(sensorTasks[sensor]);
        if(returnCode != TASK_EXECUTION_OKAY){
            Serial.print("Failed to initialize Task ");
            Serial.print(sensorTasks[sensor]->taskName);
            Serial.print(", failed with error code: ");
            Serial.println(Task_Return_Code_Names[returnCode]);
        }
    }
}