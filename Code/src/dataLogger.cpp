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
        const char* columnLabels = "Time (ms),Accel_x,Accel_y,Accel_z,Gyro_x,Gyro_y,Gyro_z,IMU_TEMP,Magnetic_x,Magnetic_y,Magnetic_z,Magnetic Degrees,Pressure,Atmosphere Temp,Solar Current In,Current Consumption,Altitute(calculated),Solar Volts,Solar Current,Solar Power,Battery Volts,Battery Current,Battery Power,Photo#";

        double rawData[SAMPLES_TO_AVERAGE][NUM_ITEMS_IN_CSV];
        unsigned row = 0;

        int calculateAltitude(double pressure){
            //TODO implement this to be more accurite
            return (1-pow(pressure/1013.25,.190284)*145366.45);
        }

        void appendData(File* destination, double* data, int photoNumber){
            
            for(byte col = 0; col < NUM_ITEMS_IN_CSV; col++){
                switch(col){
                    case ALTITUTE:
                        destination->print(calculateAltitude(data[ALTITUTE]));
                        break;
                    case PHOTO:
                        if(photoNumber){
                            destination->print(photoNumber);
                        }
                        break;
                    case TIME:
                        destination->print(millis());
                        break;
                    default:
                        destination->print(data[col]);
                        #if DEBUG
                        Serial.print(data[col]);
                        Serial.print(',');
                        #endif
                        break;
                }
                destination->print(',');
                
            }
            
            destination->println();
            
            #if DEBUG
            Serial.println();
            #endif
            
        }

        double* averageData(){
            double* dataOut = new double[NUM_ITEMS_IN_CSV];
            
            for(byte col = 0; col < NUM_ITEMS_IN_CSV - 2; col++){
                dataOut[col] = 0.0;
                for(unsigned short sample = 0; sample < SAMPLES_TO_AVERAGE; sample++){
                    dataOut[col] += rawData[sample][col];
                }
                dataOut[col] /= (double) SAMPLES_TO_AVERAGE;
            }

            return dataOut;
        }
    public:

        void init(){
            if(!SD.exists(PATH_TO_LOG)){
                SD.mkdir(PATH_TO_LOG);
            }
            if(!SD.exists(PATH_TO_LOG_POS)){
                File positionFile = SD.open(PATH_TO_LOG_POS, FILE_WRITE);
                positionFile.println(0);
                positionFile.println(0);
                positionFile.close();
            }
        }

        void addData(double* data){
            for(byte col = 0; col < NUM_ITEMS_IN_CSV; col++){
                switch(col){
                    case ALTITUTE:
                        break;
                    case PHOTO:
                        break;
                    default:
                        rawData[row][col] = data[col];
                        #if DEBUG
                        Serial.print(data[col]);
                        Serial.print(',');
                        #endif
                        break;
                }
            }
            #if DEBUG
            Serial.println();
            #endif
            row++;
            if(row >= SAMPLES_TO_AVERAGE){
                row = 0;
            }
        }

        void addToCSV(int photoNumber){
            unsigned position, fileNumber;
            bool writeHeader = false;

            
            File positionFile = SD.open(PATH_TO_LOG_POS, FILE_READ);
            positionFile.seek(0);
            fileNumber = positionFile.parseInt();
            position = positionFile.parseInt();
            positionFile.close();

            
            position++;
            if(fileNumber == 0 || position > CSV_LENGTH){
                fileNumber++;
                position = 1;
                writeHeader = true;
            }

            
            positionFile = SD.open(PATH_TO_LOG_POS, O_RDWR);
            positionFile.println(fileNumber);
            positionFile.println(position);
            positionFile.close();

            File CSV = SD.open(PATH_TO_LOG + fileNumber + ".csv", FILE_WRITE);
            if(writeHeader){
                CSV.println(columnLabels);
                #if DEBUG
                Serial.println("Creating file header");
                #endif
            }
            
            double* data = averageData();
            
            appendData(&CSV, data, photoNumber);
            CSV.close();
            delete data;
        }

};