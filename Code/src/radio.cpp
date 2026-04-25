#include "compileOptions.h"
#if RADIO_ENABLE

#include "radio.h"
#include "pinDefinitions.h"
#include <RadioLib.h>
#include <string.h>

#define RAD_RESET  1
#define RAD_IRQ    2
#define RAD_EN     0
#define MAX_TELEMETRY_PACKET_LEN 96

static SENSOR_TASK** _tasks;
static uint8_t       _numTasks;
static uint16_t      _seqNum    = 0;
static bool          _functional = false;

static SX1276 radio = new Module(RAD_CS, RAD_IRQ, RAD_RESET, RADIOLIB_NC);

static uint16_t getTelemetryPayloadLen() {
    uint16_t payloadLen = sizeof(_seqNum);

    if (_tasks == nullptr) {
        return 0;
    }

    for (uint8_t s = 0; s < _numTasks; s++) {
        if (_tasks[s] == nullptr) {
            return 0;
        }
        payloadLen += static_cast<uint16_t>(_tasks[s]->numDataTypes) * sizeof(float);
    }

    return payloadLen;
}

TASK_RETURN_CODE_T initRadio(SENSOR_TASK** tasks, uint8_t numTasks) {
    _tasks    = tasks;
    _numTasks = numTasks;

    const uint16_t payloadLen = getTelemetryPayloadLen();
    if (payloadLen == 0 || payloadLen > MAX_TELEMETRY_PACKET_LEN) {
        #if DEBUG
        Serial.print("Telemetry payload too large: ");
        Serial.println(payloadLen);
        #endif
        return TASK_INVALID_PARAMETER;
    }

    pinMode(RAD_EN, OUTPUT);
    digitalWrite(RAD_EN, LOW);

    // freq(MHz), bw(kHz), sf, cr, syncWord, power(dBm), preamble, gain
    int16_t state = radio.begin(915.0, 125.0, 7, 5, 0x12, 15, 8, 0);

    if (state != RADIOLIB_ERR_NONE) {
        #if DEBUG
        Serial.print("Radio init failed, code: ");
        Serial.println(state);
        #endif
        return TASK_EXECUTION_ERROR_HW;
    }

    radio.sleep();
    _functional = true;

    #if DEBUG
    Serial.println("Radio initialized");
    #endif

    return TASK_EXECUTION_OKAY;
}

TASK_RETURN_CODE_T transmitTelemetry() {
    if (!_functional) return TASK_EXECUTION_ERROR_HW;
    if (_tasks == nullptr) return TASK_INVALID_CONTEXT;
    const uint16_t payloadLen = getTelemetryPayloadLen();
    if (payloadLen == 0 || payloadLen > MAX_TELEMETRY_PACKET_LEN) return TASK_INVALID_PARAMETER;

    // Packet layout: [seqNum (2B)] [float telemetry fields...]
    uint8_t  buf[MAX_TELEMETRY_PACKET_LEN];
    uint16_t offset = 0;

    memcpy(buf + offset, &_seqNum, sizeof(_seqNum));
    offset += sizeof(_seqNum);

    for (uint8_t s = 0; s < _numTasks; s++) {
        if (_tasks[s] == nullptr) return TASK_INVALID_CONTEXT;
        for (uint8_t d = 0; d < _tasks[s]->numDataTypes; d++) {
            if (offset + sizeof(float) > payloadLen) {
                return TASK_INVALID_PARAMETER;
            }
            memcpy(buf + offset, &_tasks[s]->dataCSV[d], sizeof(float));
            offset += sizeof(float);
        }
    }

    digitalWrite(RAD_EN, HIGH);

    int16_t state = radio.transmit(buf, offset);

    radio.sleep();
    digitalWrite(RAD_EN, LOW);

    _seqNum++;

    if (state != RADIOLIB_ERR_NONE) {
        #if DEBUG
        Serial.print("Transmit failed, code: ");
        Serial.println(state);
        #endif
        return TASK_EXECUTION_ERROR_HW;
    } else {

            #if DEBUG
        Serial.print("Transmit success, code: ");
        Serial.println(state);
        #endif
    }
    return TASK_EXECUTION_OKAY;
}

#endif
