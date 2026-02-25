#ifndef LMSModule_SENSOR_H
#define LMSModule_SENSOR_H

#include "taskHandles.h"
#include "pinDefinitions.h"

#define LMSModule_NUM_SENSOR_VALUES 7
#define LMSModule_NUM_SAMPLE_VALUES DEFAULT_NUMBER_SAMPLE_VALUES

extern SENSOR_TASK LMSModule_Task;
/*
Sets up the context for the LMSModule task
@param sensorContext the sensor context
@returns 0 if successfull
*/
TASK_RETURN_CODE_T setupLMS(SENSOR_TASK* sensorContext);

/*
Gathers data from the LMSModule sensor
@param sensorContext the sensor context
@param exportForCSV Run statistical engine to eliminate 
outliers and average data in preparation for data storage 
into the CSV
@returns 0 if successfull
*/
TASK_RETURN_CODE_T tickLMS(SENSOR_TASK* sensorContext, bool exportForCSV);

#endif