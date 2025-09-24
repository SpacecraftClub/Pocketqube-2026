#include <Adafruit_MMC56x3.h>
#include <LSM6DSOX.h>
#include <MS5611.h>
//#include <cmath> //may be needed, not super sure
#define MAG_ALPHA 0.8
#define DEG_ALPHA 0.8
#define ACC_ALPHA 0.8
#define MS_ALPHA 0.8


//********************************************************************************************************** */
//MMC5603 functions
class MMCModule {
    private:
    bool magflag = false;
    Adafruit_MMC5603 mag = Adafruit_MMC5603(12345);
    float magnet_data[3];
    float degrees;

    void collect_mag(bool init = false){
        sensors_event_t event;
        mag.getEvent(&event);
        if (init){
            magnet_data[0] = event.magnetic.x;
            magnet_data[1] = event.magnetic.y;
            magnet_data[2] = event.magnetic.z;
        }
        else{
            magnet_data[0] = event.magnetic.x * MAG_ALPHA + magnet_data[0] * (1 - MAG_ALPHA);
            magnet_data[1] = event.magnetic.y * MAG_ALPHA + magnet_data[1] * (1 - MAG_ALPHA);
            magnet_data[2] = event.magnetic.z * MAG_ALPHA + magnet_data[2] * (1 - MAG_ALPHA);
        }
    }

    void collect_deg(bool init = false){
        sensors_event_t event;
        mag.getEvent(&event);
        if (init){
            degrees = atan2(event.magnetic.y, event.magnetic.x) * 180 / 3.14159;
        }
        else{
            degrees = degrees * DEG_ALPHA + atan2(event.magnetic.y, event.magnetic.x) * 180 / 3.14159 * (1 - DEG_ALPHA);
        }
    }

    void collect_magnet(bool init = false){ //has everything in one call
        sensors_event_t event;
        mag.getEvent(&event);
        if (init){
            magnet_data[0] = event.magnetic.x;
            magnet_data[1] = event.magnetic.y;
            magnet_data[2] = event.magnetic.z;
            degrees = atan2(event.magnetic.y, event.magnetic.x) * 180 / 3.14159;
        }
        else{
            magnet_data[0] = event.magnetic.x * MAG_ALPHA + magnet_data[0] * (1 - MAG_ALPHA);
            magnet_data[1] = event.magnetic.y * MAG_ALPHA + magnet_data[1] * (1 - MAG_ALPHA);
            magnet_data[2] = event.magnetic.z * MAG_ALPHA + magnet_data[2] * (1 - MAG_ALPHA);
            degrees = degrees * DEG_ALPHA + atan2(event.magnetic.y, event.magnetic.x) * 180 / 3.14159 * (1 - DEG_ALPHA);
        }
    }

    public:

    String init_mag(){
        if(!mag.begin(MMC56X3_DEFAULT_ADDRESS, &Wire)){
            magflag = true;
            return "MMC5603 Failure";
        }
        else {
            magflag = false;
            collect_magnet(true);
            return "MMC5603 Initialized";
        }
    }

    float* get_data(){ //x, y, z, degrees
        if (magflag) return NULL;
        static float result[4];
        result[0] = magnet_data[0];
        result[1] = magnet_data[1];
        result[2] = magnet_data[2];
        result[3] = degrees;
        return result;
    }
    
    void tick(){//select the right one here
        if (magflag) return;
        collect_magnet();
    }

};
//********************************************************************************************************** */
//LMS6DSOX functions

class LMSModule{
    private:

    bool accflag = false;
    float accel_data[3];
    float gyro_data[3];
    float temp_data;

    void collect_acc(bool init = false){
        if (!IMU.accelerationAvailable()){
            accel_data[0] = 0;
            accel_data[1] = 0;
            accel_data[2] = 0;
        } 
        if (init){
            IMU.readAcceleration(accel_data[0], accel_data[1], accel_data[2]);
        }
        else{
            float temp[3];
            IMU.readAcceleration(temp[0], temp[1], temp[2]);
            accel_data[0] = temp[0] * ACC_ALPHA + accel_data[0] * (1 - ACC_ALPHA);
            accel_data[1] = temp[1] * ACC_ALPHA + accel_data[1] * (1 - ACC_ALPHA);
            accel_data[2] = temp[2] * ACC_ALPHA + accel_data[2] * (1 - ACC_ALPHA);
        }
    }

    void collect_gyro(bool init = false){
        if (!IMU.gyroscopeAvailable()){
            gyro_data[0] = 0;
            gyro_data[1] = 0;
            gyro_data[2] = 0;
        } 
        if (init){
            IMU.readGyroscope(gyro_data[0], gyro_data[1], gyro_data[2]);
        }
        else{
            float temp[3];
            IMU.readGyroscope(temp[0], temp[1], temp[2]);
            gyro_data[0] = temp[0] * ACC_ALPHA + gyro_data[0] * (1 - ACC_ALPHA);
            gyro_data[1] = temp[1] * ACC_ALPHA + gyro_data[1] * (1 - ACC_ALPHA);
            gyro_data[2] = temp[2] * ACC_ALPHA + gyro_data[2] * (1 - ACC_ALPHA);
        }
    }

    void collect_temp(bool init = false){
        if (!IMU.temperatureAvailable()) temp_data = 0;
        if (init){
            IMU.readTemperatureFloat(temp_data);
            // if(!readSuccess) return "LSM6DSOX: Temperature not available";
        }
        else{
            float temp;
            IMU.readTemperatureFloat(temp);
            // if(!readSuccess) return "LSM6DSOX: Temperature not available";
            temp_data = temp * ACC_ALPHA + temp_data * (1 - ACC_ALPHA);
        }
    }

    void collect_all(bool init = false){
        collect_acc(init);
        collect_gyro(init);
        collect_temp(init);
    }

    public:

    float* get_data(){
        static float result[7];
        result[0] = accel_data[0];
        result[1] = accel_data[1];
        result[2] = accel_data[0];
        result[3] = gyro_data[0];
        result[4] = gyro_data[1];
        result[5] = gyro_data[2];
        result[6] = temp_data;
        return result;

    }


    String init_LSM6DOX(){
        if(!IMU.begin()){
            accflag = true;
            return "LSM6DSOX Failure";
        }
        else {
            accflag = false;
            collect_all(true);
            return "LSM6DSOX Initialized";
        }
    }
    void tick(){
        if (accflag) return;
        collect_all();
    }

};


//********************************************************************************************************** */
class MS5611Module{
    private:
    bool msflag = false;
    MS5611 ms = MS5611(0x76);
    float pressure;
    float temperature;

    void collect_pressure(bool init = false){
        ms.read();
        if (init){
            pressure = ms.getPressurePascal();
        }
        else{
            float temp;
            temp = ms.getPressurePascal();
            pressure = temp * MS_ALPHA + pressure * (1 - MS_ALPHA);
        }
    }
    void collect_temp(bool init = false){
        ms.read();
        if (init){
            temperature = ms.getTemperature();
        }
        else{
            float temp;
            temp = ms.getTemperature();
            temperature = temp * MS_ALPHA + temperature * (1 - MS_ALPHA);
        }
    }

    void collect_all(bool init = false){
        ms.read();
        if (init){
            pressure = ms.getPressurePascal();
            temperature = ms.getTemperature();
        }
        else{
            float temp;
            temp = ms.getPressurePascal();
            pressure = temp * MS_ALPHA + pressure * (1 - MS_ALPHA);
            temp = ms.getTemperature();
            temperature = temp * MS_ALPHA + temperature * (1 - MS_ALPHA);
        }
    }



    public:

    String init_MS5611(){
        if(!ms.begin()){
            msflag = true;
            return "MS5611 Failure";
        }
        else {
            msflag = false;
            ms.setOversampling(OSR_ULTRA_HIGH);// Can change to other settings
            ms.reset();
            collect_pressure(true);
            collect_temp(true);
            return "MS5611 Initialized";
        }
    }

    float* get_data(){
        static float result[2];
        if (msflag) return result;
        result[0] = pressure;
        result[1] = temperature;
        return result;
    }

    void tick(){
        if (msflag) return;
        collect_all();
    }



};

/*
#include "MS5611.h"
MS5611 MS5611(0x77);
//needs fast baud rate
MS5611.begin();
MS5611.getAddress();
M55611.reset(1); //not sure how this resets it. Maybe calls some bias getter.
MS5611.setOversampling(OSR_ULTRA_HIGH);

MS5611.read()
//should output MS5611_READ_OK
MS5611.getPressure();
MS5611.getPressurePascal();
MS5611.getTemperature();
*/

/*
  There are 5 oversampling settings, each corresponding to a different amount of milliseconds
  The higher the oversampling, the more accurate the reading will be, however the longer it will take.
  OSR_ULTRA_HIGH -> 8.22 millis
  OSR_HIGH       -> 4.11 millis
  OSR_STANDARD   -> 2.1 millis
  OSR_LOW        -> 1.1 millis
  OSR_ULTRA_LOW  -> 0.5 millis   Default = backwards compatible
*/

