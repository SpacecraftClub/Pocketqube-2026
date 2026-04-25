#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "compileOptions.h"
#include "taskHandles.h"

#if POWER_MONITOR

#define DS2782_NUM_SENSOR_VALUES 6
#define DS2782_NUM_SAMPLE_VALUES DEFAULT_NUMBER_SAMPLE_VALUES

extern SENSOR_TASK DS2782_BatteryMonitor_Task;

TASK_RETURN_CODE_T setupDS2782BatteryMonitor(SENSOR_TASK* sensorContext);
TASK_RETURN_CODE_T tickDS2782BatteryMonitor(SENSOR_TASK* sensorContext, bool exportForCSV);

// One-time calibration/configuration helpers. Leave disabled during normal flight code.
void configureDS2782BatteryMonitor();
void resetDS2782Accumulator();
void printDS2782Parameters();

#endif

#endif
