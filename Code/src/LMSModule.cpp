#include "LMSModule.h"
#include <LSM6DSOX.h>
#include <stats.h>

/*
-----LMSModule Sensor task-----
Sensor task for PocketQube sat
Monitors acceleration, gyroscope, and temperature

-----Written-----
Katherine Pearson
BYU Spacecraft Club
2-16-26
Initial creation

-----Edits-----


*/

static float data[LMSModule_NUM_SENSOR_VALUES][LMSModule_NUM_SAMPLE_VALUES];
static float dataCSV[LMSModule_NUM_SENSOR_VALUES];
static const char* dataNames[LMSModule_NUM_SENSOR_VALUES] = {"accel_x", "accel_y", "accel_z", "gyro_x", "gyro_y", "gyro_z", "temp_LMS"};
static const char* taskName = "[LMS Sensor]";

// Need to make one for the gyro data too

SENSOR_TASK LMSModule_Task = {
    (float*) data,
    dataCSV,
    dataNames,
    taskName,
    setupLMS,
    tickLMS,
    LMSModule_NUM_SENSOR_VALUES,
    LMSModule_NUM_SAMPLE_VALUES,
    0
};

TASK_RETURN_CODE_T setupLMS(SENSOR_TASK* sensorContext){
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;
    if(sensorContext == nullptr){
        retVal = TASK_INVALID_CONTEXT;
    }

    if(retVal == TASK_EXECUTION_INCOMPLETE){
        if(IMU.begin() == true){
            // What needs to be set by the IMU?
            sensorContext->numDataSamples = 0;
            retVal = TASK_EXECUTION_OKAY;
        } else {
            retVal = TASK_EXECUTION_ERROR_HW;
            sensorContext->errorFlags |= 0x01;
        }
    }

    return retVal;
};

TASK_RETURN_CODE_T tickLMS(SENSOR_TASK* sensorContext, bool exportForCSV) {
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
    if(sensorContext->numDataSamples == LMSModule_NUM_SAMPLE_VALUES - 1){
        retVal = TASK_EXECUTION_OVERTICKED;
    }

    // make sure that we didn't hit an error before this
    if(retVal == TASK_EXECUTION_INCOMPLETE || retVal == TASK_EXECUTION_OVERTICKED && exportForCSV){
        if(retVal != TASK_EXECUTION_OVERTICKED){
            float a_x, a_y, a_z, g_x, g_y, g_z;
            int temp;
            IMU.readAcceleration(a_x, a_y, a_z);
            IMU.readGyroscope(g_x, g_y, g_z);
            IMU.readTemperature(temp);
            //collect data
            data[0][sensorContext->numDataSamples] = a_x;
            data[1][sensorContext->numDataSamples] = a_y;
            data[2][sensorContext->numDataSamples] = a_z;
            data[3][sensorContext->numDataSamples] = g_x;
            data[4][sensorContext->numDataSamples] = g_y;
            data[5][sensorContext->numDataSamples] = g_z;
            data[6][sensorContext->numDataSamples] = temp;
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
                operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[3],sensorContext->numDataSamples,&dataCSV[3]));
                operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[4],sensorContext->numDataSamples,&dataCSV[4]));
                operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[5],sensorContext->numDataSamples,&dataCSV[5]));
                operationReturnCode = (StatsOperation)(operationReturnCode | CalculateMean(data[6],sensorContext->numDataSamples,&dataCSV[6]));
            } else if (sensorContext->numDataSamples == 2) {
                dataCSV[0] = (data[0][0] + data[0][1])/2;
                dataCSV[1] = (data[1][0] + data[1][1])/2;
                dataCSV[2] = (data[2][0] + data[2][1])/2;
                dataCSV[3] = (data[3][0] + data[3][1])/2;
                dataCSV[4] = (data[4][0] + data[4][1])/2;
                dataCSV[5] = (data[5][0] + data[5][1])/2;
                dataCSV[6] = (data[6][0] + data[6][1])/2;
            } else if (sensorContext->numDataSamples != 0) {
                dataCSV[0] = data[0][0];
                dataCSV[1] = data[1][0];
                dataCSV[2] = data[2][0];
                dataCSV[3] = data[3][0];
                dataCSV[4] = data[4][0];
                dataCSV[5] = data[5][0];
                dataCSV[6] = data[6][0];
            }
            //Make sure we didn't hit an error in our stat. anlys.
            if(operationReturnCode != STATS_OPERATION_OK){
                Serial.print("Stats Error Code: ");
                Serial.println(operationReturnCode);
                Serial.print("Num Items: ");
                Serial.println(sensorContext->numDataSamples);
                for(uint8_t dataType = 0; dataType < sensorContext->numDataTypes; dataType++){
                    Serial.print('\t');
                    Serial.print(sensorContext->dataNames[dataType]);
                    Serial.print(": ");
                    for(uint8_t sampleNum = 0; sampleNum < sensorContext->numDataSamples; sampleNum++){
                        Serial.print(data[dataType][sampleNum]);
                        Serial.print(",");
                    }
                    Serial.println();
                }
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
