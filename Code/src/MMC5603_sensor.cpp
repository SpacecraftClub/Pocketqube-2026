#include "MMC5603_Sensor.h"
#include <Adafruit_MMC56x3.h>
#include <stats.h>

/*
-----MMC5603 Sensor task-----
Sensor task for PocketQube sat
Monitors 3-D magnetic flux

-----Written-----
Katherine Pearson
BYU Spacecraft Club
2-10-26
Initial creation

-----Edits-----


*/

static Adafruit_MMC5603 mag = Adafruit_MMC5603(MMC5603_I2C_ADDR);

static float data[MMC5603_NUM_SENSOR_VALUES][MMC5603_NUM_SAMPLE_VALUES];
static float dataCSV[MMC5603_NUM_SENSOR_VALUES];
static const char* dataNames[MMC5603_NUM_SENSOR_VALUES] = {"x_mag", "y_mag", "z_mag"};
static const char* taskName = "[MMC5603 Sensor]";

SENSOR_TASK MMC5603_Task = {
    (float*) data,
    dataCSV,
    dataNames,
    taskName,
    setupMMC5603,
    tickMMC5603,
    MMC5603_NUM_SENSOR_VALUES,
    MMC5603_NUM_SAMPLE_VALUES,
    0
};

TASK_RETURN_CODE_T setupMMC5603(SENSOR_TASK* sensorContext) {
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;
    if(sensorContext == nullptr){
        retVal = TASK_INVALID_CONTEXT;
    }

    if(retVal == TASK_EXECUTION_INCOMPLETE){
        if(mag.begin(MMC5603_I2C_ADDR, &Wire) == true){
            sensorContext->numDataSamples = 0;
            retVal = TASK_EXECUTION_OKAY;
        } else {
            retVal = TASK_EXECUTION_ERROR_HW;
            sensorContext->errorFlags |= 0x01;
        }
    }

    return retVal;
};

TASK_RETURN_CODE_T tickMMC5603(SENSOR_TASK* sensorContext, bool exportForCSV) {
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;
    // make sure sensor context exists
    if(sensorContext == nullptr){
        retVal = TASK_INVALID_CONTEXT;
    }

    // make sure no error flags are set (HW error)
    if(sensorContext->errorFlags != 0){
        retVal = TASK_EXECUTION_ERROR_HW; // TODO: better error codes
    }

    // make sure that we have space in the buffer
    if(sensorContext->numDataSamples == MMC5603_NUM_SAMPLE_VALUES - 1){
        retVal = TASK_EXECUTION_OVERTICKED;
    }

    // make sure that we didn't hit an error before this
    if(retVal == TASK_EXECUTION_INCOMPLETE || retVal == TASK_EXECUTION_OVERTICKED && exportForCSV){
        if(retVal != TASK_EXECUTION_OVERTICKED){
            sensors_event_t event;
            mag.getEvent(&event);
            //collect data
            data[0][sensorContext->numDataSamples] = event.magnetic.x;
            data[1][sensorContext->numDataSamples] = event.magnetic.y;
            data[2][sensorContext->numDataSamples] = event.magnetic.z;
            //increment counter
            sensorContext->numDataSamples++;
        }
        //if we should prepare for the CSV
        if(exportForCSV){
            //Run stat. anlys. on data
            StatsOperation operationReturnCode = STATS_OPERATION_OK;
            if(sensorContext->numDataSamples > 2){
                operationReturnCode = CalculateMean(data[0],sensorContext->numDataSamples,&dataCSV[0]);
                operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[1],sensorContext->numDataSamples,&dataCSV[1]));
                operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[2],sensorContext->numDataSamples,&dataCSV[2]));
            } else if (sensorContext->numDataSamples == 2) {
                dataCSV[0] = (data[0][0] + data[0][1])/2;
                dataCSV[1] = (data[1][0] + data[1][1])/2;
                dataCSV[2] = (data[2][0] + data[2][1])/2;
            } else if (sensorContext->numDataSamples != 0) {
                dataCSV[0] = data[0][0];
                dataCSV[1] = data[1][0];
                dataCSV[2] = data[2][0];
            }
            //Make sure we didn't hit an error in our stat. anlys.
            if(operationReturnCode != STATS_OPERATION_OK){
                Serial.print("Stats Error Code: ");
                Serial.println(operationReturnCode);
                Serial.print("Num Items: ");
                Serial.println(sensorContext->numDataSamples);
                retVal = TASK_STATS_SUBSYS_ERROR;
            } else{
                retVal = TASK_EXECUTION_OKAY;
            }
            sensorContext->numDataSamples = 0;
        }else{
            retVal = TASK_EXECUTION_OKAY;
        }
    }

    return retVal;
};