#ifndef RADIO_H
#define RADIO_H

#include "taskHandles.h"
#include "compileOptions.h"

#if RADIO_ENABLE
TASK_RETURN_CODE_T initRadio(SENSOR_TASK** tasks, uint8_t numTasks);
TASK_RETURN_CODE_T transmitTelemetry();
#endif

#endif
