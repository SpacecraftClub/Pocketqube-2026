#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <SD.h>

#define PATH_TO_LOG String("sensors/")
#define PATH_TO_LOG_POS PATH_TO_LOG + "pos.txt"
#define CSV_LENGTH 1000
#define SAMPLES_TO_AVERAGE 10

enum columnValues : byte {
    TIME,
    ACCELERATION_X,
    ACCELERATION_Y,
    ACCELERATION_Z,
    GYRO_X,
    GYRO_Y,
    GYRO_Z,
    IMU_TEMP,
    MAGNETIC_X,
    MAGNETIC_Y,
    MAGNETIC_Z,
    MAGNETIC_DEGREES,
    PRESSURE,
    ATMOSPHERE_TEMP,
    CURRENT_IN,
    CURRENT_OUT,
    ALTITUTE,
    SOLAR_VOLTS,
    SOLAR_CURRENT,
    SOLAR_POWER,
    BATTERY_VOLTS,
    BATTERY_CURRENT,
    BATTERY_POWER,
    PHOTO,
    END_OF_CSV
};

#define NUM_ITEMS_IN_CSV END_OF_CSV

class DataPersistance{
    private:

        double rawData[SAMPLES_TO_AVERAGE][NUM_ITEMS_IN_CSV];
        unsigned row = 0;

        int calculateAltitude(double pressure);

        void appendData(File* destination, double* data, int photoNumber);

        double* averageData();
    public:

        void init();

        void addData(double* data);

        void addToCSV(int photoNumber);

};

#endif