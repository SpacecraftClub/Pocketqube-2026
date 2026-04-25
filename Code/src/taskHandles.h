#ifndef TASK_HANDLES_H
#define TASK_HANDLES_H
#include <stdint.h>

#define NUM_SENSOR_TASKS 3

#define DEFAULT_NUMBER_SAMPLE_VALUES 10

typedef enum TASK_RETURN_CODE_T : uint8_t {
    TASK_EXECUTION_OKAY = 0x00,
    TASK_INVALID_CONTEXT,
    TASK_EXECUTION_INCOMPLETE,
    TASK_EXECUTION_INTERRUPTED,
    TASK_EXECUTION_ERROR_HW,
    TASK_EXECUTION_BUS_BUSY,
    TASK_EXECUTION_OVERTICKED,
    TASK_STATS_SUBSYS_ERROR,
    TASK_INVALID_PARAMETER,
    TASK_EXECUTION_ERROR_UNSPECIFIED
} TASK_RETURN_CODE;

extern const char* Task_Return_Code_Names[];

typedef struct CAMERA_TASK {
    uint64_t lastRun_ms;
    TASK_RETURN_CODE_T (*setup)(CAMERA_TASK*);
    TASK_RETURN_CODE_T (*tick)(CAMERA_TASK*);
    uint32_t period_ms;
    uint32_t lastPhotoTaken;
} CAMERA_TASK;


typedef struct SENSOR_TASK {
    float* data;
    float* dataCSV;
    const char** dataNames;
    const char* taskName;
    TASK_RETURN_CODE_T (*setup)(SENSOR_TASK*);
    TASK_RETURN_CODE_T (*tick)(SENSOR_TASK*, bool); // the boolean is for if the data needs to be exported this tick for the CSV
    const uint8_t numDataTypes;
    uint8_t numDataSamples;
    uint8_t errorFlags; // for general use
} SENSOR_TASK;

/*
Runs a task and handles any bad outputs, printing them to debug
@param taskPtr the pointer to the task context
@param exportForCSV Run statistical engine to eliminate 
*/
void runSensorTask(SENSOR_TASK* taskPtr, bool exportForCSV);

void runSensorTasks(bool exportForCSV);

void setupSensorTasks();

extern SENSOR_TASK* sensorTasks[NUM_SENSOR_TASKS];

#endif