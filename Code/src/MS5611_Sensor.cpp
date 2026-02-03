#include "MS5611_Sensor.h"
#include <MS5611.h>
#include <stats.h>

/*
-----MS5611 Sensor task-----
Sensor task for PocketQube sat
Monitors ambient pressure and tempurature

-----Written-----
Nathan Ford
BYU Spacecraft Club
2-2-25
Initial creation

-----Edits-----


*/

static float data[MS5611_NUM_SENSOR_VALUES][MS5611_NUM_SAMPLE_VALUES];
static float dataCSV[MS5611_NUM_SENSOR_VALUES];
static const char* dataNames[MS5611_NUM_SENSOR_VALUES] = {"Pressure", "Temp (MS5611)"};
static const char* taskName = "[MS5611 Sensor]";

static MS5611 msSensor = MS5611(MS5611_I2C_ADDR);

SENSOR_TASK MS5611_Task = {
    (float*) data,
    dataCSV,
    dataNames,
    taskName,
    setupMS5611,
    tickMS5611,
    MS5611_NUM_SENSOR_VALUES,
    MS5611_NUM_SAMPLE_VALUES,
    0
};

TASK_RETURN_CODE_T setupMS5611(SENSOR_TASK* sensorContext){
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;
    if(sensorContext == nullptr){
        retVal = TASK_INVALID_CONTEXT;
    }

    if(retVal == TASK_EXECUTION_INCOMPLETE){
        if(msSensor.begin() == true){
            msSensor.setOversampling(OSR_ULTRA_HIGH); // Can change to other settings
            msSensor.reset();
            retVal = TASK_EXECUTION_OKAY;
        } else {
            retVal = TASK_EXECUTION_ERROR_HW;
            sensorContext->errorFlags |= 0x01;
        }
    }

    return retVal;
}
#include <Arduino.h>
TASK_RETURN_CODE_T tickMS5611(SENSOR_TASK* sensorContext, bool exportForCSV){
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
    if(sensorContext->numDataSamples == MS5611_NUM_SAMPLE_VALUES - 1){
        retVal = TASK_EXECUTION_OVERTICKED;
    }

    // make sure that we didn't hit an error before this
    if(retVal == TASK_EXECUTION_INCOMPLETE || retVal == TASK_EXECUTION_OVERTICKED && exportForCSV){
        if(retVal != TASK_EXECUTION_OVERTICKED){
            msSensor.read();
            //collect data
            data[0][sensorContext->numDataSamples] = msSensor.getPressurePascal();
            data[1][sensorContext->numDataSamples] = msSensor.getTemperature();
            //increment counter
            sensorContext->numDataSamples++;
        }
        //if we should prepare for the CSV
        if(exportForCSV){
            //Run stat. anlys. on data
            StatsOperation operationReturnCode;
            operationReturnCode = CalculateMean(data[0],sensorContext->numDataSamples,&dataCSV[0]);
            operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[1],sensorContext->numDataSamples,&dataCSV[1]));
            //Make sure we didn't hit an error in our stat. anlys.
            if(operationReturnCode != STATS_OPERATION_OK){
                msSensor.getAddress();
                Serial.println(operationReturnCode);
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
}