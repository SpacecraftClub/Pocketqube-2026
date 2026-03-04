#ifndef SENSOR_LOGGER_H
#define SENSOR_LOGGER_H

#include "taskHandles.h"


TASK_RETURN_CODE_T initializeLogger(SENSOR_TASK* sensorTasks);

TASK_RETURN_CODE_T logNewData();



#endif