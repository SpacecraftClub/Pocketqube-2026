#ifndef MMC5603_SENSOR_H
#define MMC5603_SENSOR_H

#include "taskHandles.h"
#include "pinDefinitions.h"

#define MMC5603_NUM_SENSOR_VALUES 3
#define MMC5603_NUM_SAMPLE_VALUES DEFAULT_NUMBER_SAMPLE_VALUES


extern SENSOR_TASK MMC5603_Task;
/*
Sets up the context for the MMC5603 task
@param sensorContext the sensor context
@returns 0 if successfull
*/
TASK_RETURN_CODE_T setupMMC5603(SENSOR_TASK* sensorContext);

/*
Gathers data from the MMC5603 sensor
@param sensorContext the sensor context
@param exportForCSV Run statistical engine to eliminate 
outliers and average data in preparation for data storage 
into the CSV
@returns 0 if successfull
*/
TASK_RETURN_CODE_T tickMMC5603(SENSOR_TASK* sensorContext, bool exportForCSV);

#endif