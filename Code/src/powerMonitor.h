// File containing functions to monitor power on pocketcube using 3 IN260 current sensors
#ifndef POWER_MONITOR_H
#define POWER_MONITOR_H

#include <Adafruit_INA260.h>
#include <Wire.h>  // Required for I2C communication

#define SOLAR_SENSOR_ADDR 0x40  // Default I2C address, A0=A1=GND
#define BATT_SENSOR_ADDR 0x45  // I2C address A0=A1=VCC

enum sensorName { SOLAR_SENSOR, BATTERY_SENSOR };

// Define INA260 instances for each sensor
class PowerMonitor{
  private:
  Adafruit_INA260 solarSensor;
  Adafruit_INA260 battSensor;

  // enum for sensor names to be used in one shot current and voltage measurements
  public:

  // Initialize all sensors
  bool initSensors() {
      bool success = true;

      if (!solarSensor.begin(SOLAR_SENSOR_ADDR)) {
          Serial.println("Failed to find Solar INA260!");
          success = false;
      }

      if (!battSensor.begin(BATT_SENSOR_ADDR)) {
          Serial.println("Failed to find Battery INA260!");
          success = false;
      }

      // Set sensors mode
      solarSensor.setMode(INA260_MODE_CONTINUOUS);
      battSensor.setMode(INA260_MODE_CONTINUOUS);
      return success;
  }

  // Single sample functions
  float readCurrent(sensorName sensor) {
    switch (sensor) {
      case SOLAR_SENSOR:
        return solarSensor.readCurrent();  // Read current from solar sensor
      case BATTERY_SENSOR:
        return battSensor.readCurrent();  // Read current from battery sensor
      default:
        return -1.0;  // Error case if sensor is not valid
    }
  }

  float readVoltage(sensorName sensor) {
    switch (sensor) {
      case SOLAR_SENSOR:
        return solarSensor.readBusVoltage();  // Read voltage from solar sensor
      case BATTERY_SENSOR:
        return battSensor.readBusVoltage();  // Read voltage from battery sensor
      default:
        return -1.0;  // Error case if sensor is not valid
    }
  }

  float readPower(sensorName sensor) {
    switch (sensor) {
      case SOLAR_SENSOR:
        return solarSensor.readPower();  // Read power from solar sensor
      case BATTERY_SENSOR:
        return battSensor.readPower();  // Read power from battery sensor
      default:
        return -1.0;  // Error case if sensor is not valid
    }
  }
};

#endif

