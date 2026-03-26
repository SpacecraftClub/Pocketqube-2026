#ifndef SENSOR_LOGGER_H
#define SENSOR_LOGGER_H

#include "camera.h"

#include "taskHandles.h"

#define PATH_TO_LOG String("sensors/")
#define PATH_TO_LOG_POS PATH_TO_LOG + "pos.txt"
#define CSV_LENGTH 1000

/*
Initialize SD Card logger creating file paths as necessary
@param sensorTasks tasks to log
@returns status of the operation
*/
TASK_RETURN_CODE_T initializeLogger(SENSOR_TASK** sensorTasks, CAMERA_TASK* cameraTask);

TASK_RETURN_CODE_T logNewData();



#endif