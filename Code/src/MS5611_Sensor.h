#ifndef MS5611_SENSOR_H
#define MS5611_SENSOR_H

#include "taskHandles.h"
#include "pinDefinitions.h"

#define MS5611_NUM_SENSOR_VALUES 2
#define MS5611_NUM_SAMPLE_VALUES DEFAULT_NUMBER_SAMPLE_VALUES


extern SENSOR_TASK MS5611_Task;
/*
Sets up the context for the MS5611 task
@param sensorContext the sensor context
@returns 0 if successfull
*/
TASK_RETURN_CODE_T setupMS5611(SENSOR_TASK* sensorContext);

/*
Gathers data from the MS5611 sensor
@param sensorContext the sensor context
@param exportForCSV Run statistical engine to eliminate 
outliers and average data in preparation for data storage 
into the CSV
@returns 0 if successfull
*/
TASK_RETURN_CODE_T tickMS5611(SENSOR_TASK* sensorContext, bool exportForCSV);

#endif