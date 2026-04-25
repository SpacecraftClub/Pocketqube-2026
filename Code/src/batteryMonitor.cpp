#include "batteryMonitor.h"

#if POWER_MONITOR

#include <Arduino.h>
#include <Wire.h>
#include <stats.h>

namespace {

constexpr uint8_t DS2782_ADDR = 0x34;
constexpr float DS2782_RSENSE_OHMS = 0.01f;
constexpr float DS2782_BATTERY_CAPACITY_MAH = 1400.0f;
constexpr bool DS2782_CONFIGURE_ON_BOOT = false;
constexpr bool DS2782_RESET_ACR_ON_BOOT = false;

constexpr uint8_t REG_COULOMB_COUNTER = 0x06;
constexpr uint8_t REG_VOLTAGE = 0x0C;
constexpr uint8_t REG_CURRENT = 0x0E;

enum BatteryDataIndex : uint8_t {
    BATTERY_VOLTAGE = 0,
    BATTERY_CURRENT,
    BATTERY_POWER,
    BATTERY_ENERGY,
    BATTERY_CAPACITY,
    BATTERY_SOC
};

float data[DS2782_NUM_SENSOR_VALUES][DS2782_NUM_SAMPLE_VALUES];
float dataCSV[DS2782_NUM_SENSOR_VALUES];
const char* dataNames[DS2782_NUM_SENSOR_VALUES] = {
    "batt_voltage",
    "batt_current",
    "batt_power",
    "batt_energy",
    "batt_capacity",
    "batt_soc"
};
const char* taskName = "[DS2782 Battery Monitor]";

unsigned long lastMeasurementMillis = 0;
float cumulativeEnergyJ = 0.0f;

bool readRegisterBlock(uint8_t address, uint8_t* buffer, uint8_t length) {
    Wire.beginTransmission(DS2782_ADDR);
    Wire.write(address);
    if (Wire.endTransmission(false) != 0) {
        return false;
    }

    if (Wire.requestFrom(DS2782_ADDR, length) != length) {
        return false;
    }

    for (uint8_t i = 0; i < length; i++) {
        buffer[i] = Wire.read();
    }

    return true;
}

bool writeEEPROM(uint8_t address, uint8_t value) {
    Wire.beginTransmission(DS2782_ADDR);
    Wire.write(address);
    Wire.write(value);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    delay(10);

    Wire.beginTransmission(DS2782_ADDR);
    Wire.write(0x48);
    Wire.write(address);
    if (Wire.endTransmission() != 0) {
        return false;
    }
    delay(20);

    return true;
}

uint8_t readRegister(uint8_t address) {
    uint8_t value = 0xFF;
    (void) readRegisterBlock(address, &value, 1);
    return value;
}

bool readVoltage(float* voltageV) {
    uint8_t raw[2];
    if (voltageV == nullptr || !readRegisterBlock(REG_VOLTAGE, raw, sizeof(raw))) {
        return false;
    }

    const uint16_t rawVoltage = (static_cast<uint16_t>(raw[0]) << 3) | (raw[1] >> 5);
    *voltageV = rawVoltage * 0.00488f;
    return true;
}

bool readCurrent(float* currentA) {
    uint8_t raw[2];
    if (currentA == nullptr || !readRegisterBlock(REG_CURRENT, raw, sizeof(raw))) {
        return false;
    }

    const int16_t rawCurrent = static_cast<int16_t>(
        (static_cast<uint16_t>(raw[0]) << 8) | raw[1]
    );
    *currentA = rawCurrent * 1.5625e-6f / DS2782_RSENSE_OHMS;
    return true;
}

bool readCoulombCounter(int16_t* counter) {
    uint8_t raw[2];
    if (counter == nullptr || !readRegisterBlock(REG_COULOMB_COUNTER, raw, sizeof(raw))) {
        return false;
    }

    *counter = static_cast<int16_t>((static_cast<uint16_t>(raw[0]) << 8) | raw[1]);
    return true;
}

void finalizeBatteryExport(SENSOR_TASK* sensorContext, TASK_RETURN_CODE_T* retVal) {
    if (sensorContext == nullptr || retVal == nullptr) {
        return;
    }

    if (sensorContext->numDataSamples == 0) {
        *retVal = TASK_EXECUTION_OKAY;
        return;
    }

    if (sensorContext->numDataSamples > 2) {
        StatsOperation statsCode = STATS_OPERATION_OK;
        statsCode = CalculateMean(data[BATTERY_VOLTAGE], sensorContext->numDataSamples, &dataCSV[BATTERY_VOLTAGE]);
        statsCode = static_cast<StatsOperation>(
            statsCode | CalculateMean(data[BATTERY_CURRENT], sensorContext->numDataSamples, &dataCSV[BATTERY_CURRENT])
        );
        statsCode = static_cast<StatsOperation>(
            statsCode | CalculateMean(data[BATTERY_POWER], sensorContext->numDataSamples, &dataCSV[BATTERY_POWER])
        );

        if (statsCode != STATS_OPERATION_OK) {
            *retVal = TASK_STATS_SUBSYS_ERROR;
            sensorContext->numDataSamples = 0;
            return;
        }
    } else if (sensorContext->numDataSamples == 2) {
        dataCSV[BATTERY_VOLTAGE] = (data[BATTERY_VOLTAGE][0] + data[BATTERY_VOLTAGE][1]) / 2.0f;
        dataCSV[BATTERY_CURRENT] = (data[BATTERY_CURRENT][0] + data[BATTERY_CURRENT][1]) / 2.0f;
        dataCSV[BATTERY_POWER] = (data[BATTERY_POWER][0] + data[BATTERY_POWER][1]) / 2.0f;
    } else {
        dataCSV[BATTERY_VOLTAGE] = data[BATTERY_VOLTAGE][0];
        dataCSV[BATTERY_CURRENT] = data[BATTERY_CURRENT][0];
        dataCSV[BATTERY_POWER] = data[BATTERY_POWER][0];
    }

    const uint8_t latestSample = sensorContext->numDataSamples - 1;
    dataCSV[BATTERY_ENERGY] = data[BATTERY_ENERGY][latestSample];
    dataCSV[BATTERY_CAPACITY] = data[BATTERY_CAPACITY][latestSample];
    dataCSV[BATTERY_SOC] = data[BATTERY_SOC][latestSample];

    sensorContext->numDataSamples = 0;
    *retVal = TASK_EXECUTION_OKAY;
}

}  // namespace

SENSOR_TASK DS2782_BatteryMonitor_Task = {
    reinterpret_cast<float*>(data),
    dataCSV,
    dataNames,
    taskName,
    setupDS2782BatteryMonitor,
    tickDS2782BatteryMonitor,
    DS2782_NUM_SENSOR_VALUES,
    DS2782_NUM_SAMPLE_VALUES,
    0
};

TASK_RETURN_CODE_T setupDS2782BatteryMonitor(SENSOR_TASK* sensorContext) {
    if (sensorContext == nullptr) {
        return TASK_INVALID_CONTEXT;
    }

    sensorContext->numDataSamples = 0;
    sensorContext->errorFlags = 0;
    lastMeasurementMillis = millis();
    cumulativeEnergyJ = 0.0f;

    if (DS2782_CONFIGURE_ON_BOOT) {
        configureDS2782BatteryMonitor();
    }

    if (DS2782_RESET_ACR_ON_BOOT) {
        resetDS2782Accumulator();
    }

    float voltage = 0.0f;
    float current = 0.0f;
    int16_t counter = 0;
    if (!readVoltage(&voltage) || !readCurrent(&current) || !readCoulombCounter(&counter)) {
        sensorContext->errorFlags |= 0x01;
        return TASK_EXECUTION_ERROR_HW;
    }

    return TASK_EXECUTION_OKAY;
}

TASK_RETURN_CODE_T tickDS2782BatteryMonitor(SENSOR_TASK* sensorContext, bool exportForCSV) {
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;

    if (sensorContext == nullptr) {
        return TASK_INVALID_CONTEXT;
    }

    if (sensorContext->errorFlags != 0) {
        return TASK_EXECUTION_ERROR_HW;
    }

    if (sensorContext->numDataSamples == DS2782_NUM_SAMPLE_VALUES - 1) {
        retVal = TASK_EXECUTION_OVERTICKED;
    }

    if (retVal == TASK_EXECUTION_INCOMPLETE || (retVal == TASK_EXECUTION_OVERTICKED && exportForCSV)) {
        if (retVal != TASK_EXECUTION_OVERTICKED) {
            const unsigned long currentMillis = millis();
            const float deltaTimeS = (currentMillis - lastMeasurementMillis) / 1000.0f;
            lastMeasurementMillis = currentMillis;

            float voltage = 0.0f;
            float current = 0.0f;
            int16_t coulombCounter = 0;
            if (!readVoltage(&voltage) || !readCurrent(&current) || !readCoulombCounter(&coulombCounter)) {
                sensorContext->errorFlags |= 0x01;
                return TASK_EXECUTION_ERROR_HW;
            }

            const float power = voltage * current;
            cumulativeEnergyJ += power * deltaTimeS;

            const float capacitymAh = coulombCounter * (1.0f / 65536.0f) * (1000.0f / DS2782_RSENSE_OHMS);
            const float socPercent = (capacitymAh / DS2782_BATTERY_CAPACITY_MAH) * 100.0f;

            data[BATTERY_VOLTAGE][sensorContext->numDataSamples] = voltage;
            data[BATTERY_CURRENT][sensorContext->numDataSamples] = current;
            data[BATTERY_POWER][sensorContext->numDataSamples] = power;
            data[BATTERY_ENERGY][sensorContext->numDataSamples] = cumulativeEnergyJ;
            data[BATTERY_CAPACITY][sensorContext->numDataSamples] = capacitymAh;
            data[BATTERY_SOC][sensorContext->numDataSamples] = socPercent;
            sensorContext->numDataSamples++;
        }

        if (exportForCSV) {
            finalizeBatteryExport(sensorContext, &retVal);
        } else {
            retVal = TASK_EXECUTION_OKAY;
        }
    }

    return retVal;
}

void configureDS2782BatteryMonitor() {
    const bool configured =
        writeEEPROM(0x69, 5) &&
        writeEEPROM(0x6A, 210) &&
        writeEEPROM(0x6B, 112) &&
        writeEEPROM(0x6C, 154) &&
        writeEEPROM(0x6D, 100) &&
        writeEEPROM(0x6E, static_cast<uint8_t>((44800 >> 8) & 0xFF)) &&
        writeEEPROM(0x6F, static_cast<uint8_t>(44800 & 0xFF)) &&
        writeEEPROM(0x70, 0x80);

    #if DEBUG
    if (configured) {
        Serial.println("DS2782 parameters configured");
    } else {
        Serial.println("DS2782 configuration failed");
    }
    #endif
}

void resetDS2782Accumulator() {
    Wire.beginTransmission(DS2782_ADDR);
    Wire.write(0x10);
    Wire.write(0x00);
    Wire.write(0x00);
    (void) Wire.endTransmission();
}

void printDS2782Parameters() {
    #if DEBUG
    Serial.println("---- DS2782 EEPROM Parameters ----");
    Serial.print("RSNSP: ");
    Serial.println(readRegister(0x69));
    Serial.print("VCHG: ");
    Serial.println(readRegister(0x6A));
    Serial.print("IMIN: ");
    Serial.println(readRegister(0x6B));
    Serial.print("VAE: ");
    Serial.println(readRegister(0x6C));
    Serial.print("IAE: ");
    Serial.println(readRegister(0x6D));

    const uint16_t ac = (static_cast<uint16_t>(readRegister(0x6E)) << 8) | readRegister(0x6F);
    Serial.print("AC (raw): ");
    Serial.println(ac);

    Serial.print("AS: ");
    Serial.println(readRegister(0x70));
    Serial.println("----------------------------------");
    #endif
}

#endif
