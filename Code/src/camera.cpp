#include <ArduCAM.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include "pinDefinitions.h"

#define CAMERA_DIRECTORY String("Captures/")
#define CAMERA_POSITION CAMERA_DIRECTORY + "pos.txt"
#define CAMERA_CONFIG "camera.cfg"
#define DEFAULT_CAMERA_ROLLOVER 4000

class CameraManager {
  private:
    ArduCAM myCAM;
    int cameraRollover = DEFAULT_CAMERA_ROLLOVER;

    void prepareSDCard() {
      if(!SD.exists(CAMERA_DIRECTORY)){
        SD.mkdir(CAMERA_DIRECTORY);
      }
      if(!SD.exists(CAMERA_POSITION)){
        File cameraPosition = SD.open(CAMERA_POSITION, FILE_WRITE);
        cameraPosition.println("0");
        cameraPosition.close();
      }
      if(SD.exists(CAMERA_CONFIG)){
          File config = SD.open(CAMERA_CONFIG, FILE_READ);
          while(config.available()){
              String object = String(config.readStringUntil(':'));
              if(object.indexOf("\"CameraRollover\"") != -1){
                  cameraRollover = config.parseInt();
              }
          }
      }
      else{
          File config = SD.open(CAMERA_CONFIG, FILE_WRITE);
          config.println("{");
          config.print("\t\"CameraRollover\" : ");
          config.println(cameraRollover);
          config.println("}");
          config.close();
      }
    }

    void CAMSaveToSDFile(String filePath, String fileName) {
      byte buf[256];
      static int i = 0;
      uint8_t temp = 0,temp_last=0;
      uint32_t length = 0;
      bool is_header = false;
      File outFile;
      //Flush the FIFO
      
      myCAM.flush_fifo();
      //Clear the capture done flag
      myCAM.clear_fifo_flag();
      //Start capture
      myCAM.start_capture();

      while(!myCAM.get_bit(ARDUCHIP_TRIG , CAP_DONE_MASK)){
        delayMicroseconds(1); //give a chance for the transmitter to recieve a message.
        
      }
      length = myCAM.read_fifo_length();
      

      if (length >= MAX_FIFO_SIZE) //384K
      {
        return ;
      }
      if (length == 0 ) //0 kb
      {
        return ;
      }
      //Construct a file name
      //Open the new file
      
      outFile = SD.open(filePath + fileName, O_WRITE | O_CREAT | O_TRUNC);
      if(!outFile){
        #if DEBUG
          Serial.println(F("File open faild"));
        #endif
        return;
      }
      myCAM.CS_LOW();
      myCAM.set_fifo_burst();
      while ( length-- )
      {
        temp_last = temp;
        temp =  SPI.transfer(0x00);
        //Read JPEG data from FIFO
        if ( (temp == 0xD9) && (temp_last == 0xFF) ) //If find the end ,break while,
        {
          buf[i++] = temp;  //save the last  0XD9     
          //Write the remain bytes in the buffer
          myCAM.CS_HIGH();
          outFile.write(buf, i);    
          //Close the file
          outFile.close();
          is_header = false;
          i = 0;
        }  
        if (is_header == true)
        { 
          //Write image data to buffer if not full
          if (i < 256){
            
            buf[i++] = temp;
            
          }
          else
          {
            //Write 256 bytes image data to file
            myCAM.CS_HIGH();
            outFile.write(buf, 256);
            
            i = 0;
            buf[i++] = temp;
            
            myCAM.CS_LOW();
            myCAM.set_fifo_burst();
          }        
        }
        else if ((temp == 0xD8) & (temp_last == 0xFF))
        {
          
          is_header = true;
          buf[i++] = temp_last;
          buf[i++] = temp;
          
        } 
      }  
      
    }
  
  public:

    int takePicture() {
      static int photoNumber;

      //read photo nubmer offset, so that we never accidentally write over photos
      
      File positionReader = SD.open(CAMERA_POSITION, FILE_READ);
      positionReader.seek(0);
      photoNumber = positionReader.parseInt();
      positionReader.close();
      

      photoNumber++;
      //just so we never overflow the card with photos, the amount of saved photos can be defined in the config file
      if(photoNumber > cameraRollover){ 
        photoNumber = 1;
      }

      
      File positionWriter = SD.open(CAMERA_POSITION, O_RDWR);
      positionWriter.seek(0);
      positionWriter.print(photoNumber);
      positionWriter.close();
      

      //take and save the photo
      CAMSaveToSDFile(CAMERA_DIRECTORY, String(photoNumber) + ".jpg");
      return photoNumber;
    }

    void setup(int chipSelect) {
      myCAM = ArduCAM( OV2640, chipSelect);
      uint8_t vid, pid;
      uint8_t temp;
      Wire.begin();
      //set the CS as an output:
      pinMode(chipSelect,OUTPUT);
      digitalWrite(chipSelect, HIGH);
      // initialize SPI:
      SPI.begin();
        
      //Reset the CPLD
      myCAM.write_reg(0x07, 0x80);
      delay(100);
      myCAM.write_reg(0x07, 0x00);
      delay(100);
  
      while(1){
        //Check if the ArduCAM SPI bus is OK
        myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
        temp = myCAM.read_reg(ARDUCHIP_TEST1);
      
        if (temp != 0x55){
          #if DEBUG
            Serial.println(F("SPI interface Error!"));
          #endif
          delay(1000);continue;
        }else{
          #if DEBUG
            Serial.println(F("SPI interface OK."));
          #endif
          break;
        }
      }

      while(1){
        //Check if the camera module type is OV2640
        myCAM.wrSensorReg8_8(0xff, 0x01);
        myCAM.rdSensorReg8_8(OV2640_CHIPID_HIGH, &vid);
        myCAM.rdSensorReg8_8(OV2640_CHIPID_LOW, &pid);
        if ((vid != 0x26 ) && (( pid != 0x41 ) || ( pid != 0x42 ))){
          Serial.println(F("Can't find OV2640 module!"));
          delay(1000);continue;
        }
        else{
          Serial.println(F("OV2640 detected."));break;
        } 
      }
    
      myCAM.set_format(JPEG);
      myCAM.InitCAM();
      
      myCAM.OV2640_set_JPEG_size(OV2640_1600x1200);

      myCAM.write_reg(0x13, 0xc7 ^ 0x01); // this is supposed to alter the exposure?
      myCAM.write_reg(0x45, 0x00 );
      myCAM.write_reg(0x10, 0x09 );
      myCAM.write_reg(0x04, 0x20 );

      prepareSDCard();
    }
};