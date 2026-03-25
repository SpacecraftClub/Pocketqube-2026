#include "sensor_logger.h"
#include <SdFat.h>
extern SdFs SD_CARD;

static SENSOR_TASK* sensorTasksPtr;
static CAMERA_TASK* cameraTaskPtr;

TASK_RETURN_CODE_T initializeLogger(SENSOR_TASK* sensorTasks, CAMERA_TASK* cameraTask) {
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;
    sensorTasksPtr = sensorTasks;
    cameraTaskPtr = cameraTask;
    if (sensorTasksPtr == NULL || cameraTaskPtr == NULL) {
        retVal = TASK_INVALID_PARAMETER;
    } else if(!SD_CARD.exists("/")) {
        retVal = TASK_EXECUTION_ERROR_HW;
    } else {
        if(!SD_CARD.exists(PATH_TO_LOG)){
            SD_CARD.mkdir(PATH_TO_LOG);
        }
        if(!SD_CARD.exists(PATH_TO_LOG_POS)){
            FsFile positionFile = SD_CARD.open(PATH_TO_LOG_POS, FILE_WRITE);
            positionFile.println(0);
            positionFile.println(0);
            positionFile.close();
        }
    }
    return retVal;
}

TASK_RETURN_CODE_T logNewData() {
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;

    unsigned position, fileNumber;
    bool writeHeader = false;
    
    FsFile positionFile = SD_CARD.open(PATH_TO_LOG_POS, FILE_READ);
    positionFile.seek(0);
    fileNumber = positionFile.parseInt();
    position = positionFile.parseInt();
    positionFile.close();
    
    position++;
    if(fileNumber == 0 || position > CSV_LENGTH){
        fileNumber++;
        position = 1;
        writeHeader = true;
    }
    
    positionFile = SD_CARD.open(PATH_TO_LOG_POS, O_RDWR);
    positionFile.println(fileNumber);
    positionFile.println(position);
    positionFile.close();
    
    FsFile CSV = SD_CARD.open(PATH_TO_LOG + fileNumber + ".csv", FILE_WRITE);

    if(writeHeader) {
        for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
            for(uint8_t data = 0; data < sensorTasks[sensor]->numDataTypes; data++){
                CSV.print(sensorTasks[sensor]->dataNames[data]);
                if(sensor != NUM_SENSOR_TASKS - 1 || data != sensorTasks[sensor]->numDataTypes - 1){
                    CSV.print(',');
                }
            }
        }
        CSV.println("Photo #");
    }

    for(uint8_t sensor = 0; sensor < NUM_SENSOR_TASKS; sensor++){
        for(uint8_t data = 0; data < sensorTasks[sensor]->numDataTypes; data++){
            CSV.print(sensorTasks[sensor]->dataCSV[data]);
            CSV.print(',');
        }
    }

    if(cameraTaskPtr->lastPhotoTaken != -1) {
        CSV.print(cameraTaskPtr->lastPhotoTaken);
        cameraTaskPtr->lastPhotoTaken = -1;
    }

    CSV.println();

    return retVal;
}