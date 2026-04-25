# PocketQube Flight Code

This firmware collects sensor data, logs it to the SD card, and transmits telemetry over LoRa radio.

## Viewing Data

There are three output paths:

1. Serial monitor
   Set `PRINT_SENSOR_CSV` to `true` in `src/main.cpp`. The firmware will print one CSV row per second at `9600` baud.

2. SD card logs
   Sensor data is written to CSV files by `src/sensor_logger.cpp`. Each row contains the current exported values for every sensor task.

3. Radio telemetry
   `src/radio.cpp` sends a binary packet containing a 2-byte sequence number followed by all exported sensor floats in task order.

## Sensor Task Order

Radio telemetry and CSV column order both follow the `sensorTasks[]` array in `src/taskHandles.cpp`.

Current task order:

1. `MS5611_Task`
2. `MMC5603_Task`
3. `LMSModule_Task`
4. `DS2782_BatteryMonitor_Task` when `POWER_MONITOR` is enabled

## Telemetry Field Order

### Base 12-float telemetry layout

If the battery monitor is not included in the radio payload, the 12 floats are:

1. `Pressure`
2. `Temp (MS5611)`
3. `x_mag`
4. `y_mag`
5. `z_mag`
6. `accel_x`
7. `accel_y`
8. `accel_z`
9. `gyro_x`
10. `gyro_y`
11. `gyro_z`
12. `temp_LMS`

Example:

`Telemetry: SEQ=9 Floats(12): 84966.555 23.303 913.164 1053.115 592.743 -0.002 0.120 1.024 -0.366 0.149 -0.549 20.222`

This maps to:

1. `84966.555` = `Pressure`
2. `23.303` = `Temp (MS5611)`
3. `913.164` = `x_mag`
4. `1053.115` = `y_mag`
5. `592.743` = `z_mag`
6. `-0.002` = `accel_x`
7. `0.120` = `accel_y`
8. `1.024` = `accel_z`
9. `-0.366` = `gyro_x`
10. `0.149` = `gyro_y`
11. `-0.549` = `gyro_z`
12. `20.222` = `temp_LMS`

### Battery monitor fields

When `POWER_MONITOR` is enabled, the DS2782 task adds these fields after `temp_LMS`:

13. `batt_voltage`
14. `batt_current`
15. `batt_power`
16. `batt_energy`
17. `batt_capacity`
18. `batt_soc`

## Likely Units

1. `Pressure`: Pa
2. `Temp (MS5611)`: C
3. `x_mag`, `y_mag`, `z_mag`: uT
4. `accel_x`, `accel_y`, `accel_z`: g
5. `gyro_x`, `gyro_y`, `gyro_z`: deg/s
6. `temp_LMS`: C
7. `batt_voltage`: V
8. `batt_current`: A
9. `batt_power`: W
10. `batt_energy`: J
11. `batt_capacity`: mAh
12. `batt_soc`: percent

## Important Radio Note

`src/radio.cpp` now computes the telemetry payload length from the enabled sensor tasks and checks it against `MAX_TELEMETRY_PACKET_LEN = 96`.

That is enough for the current telemetry set:

1. `2` bytes of sequence number
2. `18` floats = `72` bytes
3. Total `74` bytes

If more sensor fields are added later and the payload grows past `96` bytes, radio initialization will fail until the limit is increased.
