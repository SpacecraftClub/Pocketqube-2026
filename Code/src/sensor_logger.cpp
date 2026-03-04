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
    return retVal;
}