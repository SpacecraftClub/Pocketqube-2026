#include "camera.h"
#include "pinDefinitions.h"
#include "taskHandles.h"
#include <Arducam_Mega.h>
#include <string.h>
#include "compileOptions.h"

static Arducam_Mega myCAM(CAM_CS);
static uint8_t errorFlag = 0;

static uint32_t cameraRollover = DEFAULT_CAMERA_ROLLOVER;

extern SdFs SD_CARD;

CAMERA_TASK arducamMegaCameraContext = {
    0,
    initCamera,
    tickCamera,
    5000,
    0
};


TASK_RETURN_CODE_T initCamera(CAMERA_TASK* context){
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;

    CamStatus cameraErrorCode = myCAM.begin();

    if(cameraErrorCode != CAM_ERR_SUCCESS){
        retVal = TASK_EXECUTION_ERROR_HW;
        errorFlag |= 0x01;
    } else{
        myCAM.lowPowerOn();
        retVal = TASK_EXECUTION_OKAY;
        // #if SD_ENABLE
        if(!SD_CARD.exists(CAMERA_DIRECTORY)){
            SD_CARD.mkdir(CAMERA_DIRECTORY);
        }
        if(!SD_CARD.exists(CAMERA_POSITION)){
            #if DEBUG
                Serial.println("Creating New Camera Position File");
            #endif
            FsFile positionFile = SD_CARD.open(CAMERA_POSITION, O_CREAT | O_WRONLY);
            positionFile.println(1);
            positionFile.flush();
            positionFile.close();
        }
        if(!SD_CARD.exists(CAMERA_CONFIG)){
            #if DEBUG
                Serial.println("Creating New Camera Configuration File");
            #endif
            FsFile cameraConfig = SD_CARD.open(CAMERA_CONFIG, O_CREAT | O_WRONLY);
            cameraConfig.println("{");
            cameraConfig.print("\t\"CameraRollover\" : ");
            cameraConfig.println(cameraRollover);
            cameraConfig.println("}");
            cameraConfig.close();
            cameraConfig.flush();
            cameraConfig.close();
        } else {
            FsFile cameraConfig = SD_CARD.open(CAMERA_CONFIG, O_READ);
            cameraConfig.rewind();
            while(cameraConfig.available()){
                String object = String(cameraConfig.readStringUntil(':'));
                if(object.indexOf("\"CameraRollover\"") != -1){
                    cameraRollover = cameraConfig.parseInt();
                }
            }
            cameraConfig.close();
        }
        // #endif
    }

    return retVal;
}

TASK_RETURN_CODE_T tickCamera(CAMERA_TASK* context){
    TASK_RETURN_CODE_T retVal = TASK_EXECUTION_INCOMPLETE;
    FsFile positionFile = SD_CARD.open(CAMERA_POSITION, O_READ);
    positionFile.rewind();
    context->lastPhotoTaken = positionFile.parseInt();
    positionFile.close();
    positionFile = SD_CARD.open(CAMERA_POSITION, O_CREAT | O_WRONLY);
    positionFile.rewind();
    // positionFile.truncate();
    positionFile.println(context->lastPhotoTaken + 1);
    positionFile.flush();
    positionFile.close();

    if(context->lastPhotoTaken > cameraRollover){
        context->lastPhotoTaken = 1;
    }

    if(errorFlag){
        retVal = TASK_EXECUTION_ERROR_HW;
    } else{
        myCAM.lowPowerOff();
        CamStatus cameraErrorCode = myCAM.takePicture(CAMERA_IMAGE_QUALITY, CAM_IMAGE_PIX_FMT_JPG);
        if(cameraErrorCode != CAM_ERR_SUCCESS){
            retVal = TASK_EXECUTION_ERROR_HW;
        } else{
            #if DEBUG
            Serial.println("Capturing Photo");
            Serial.print("PhotoLen: ");
            Serial.println(myCAM.getTotalLength());
            Serial.print("Photo Number: ");
            Serial.println(context->lastPhotoTaken);
            Serial.print("File Path: ");
            Serial.println(CAMERA_PHOTO_FILE_PATH(context->lastPhotoTaken));
            #endif
            uint8_t buff[CAMERA_BUFF_LEN];
            FsFile photo = SD_CARD.open(CAMERA_PHOTO_FILE_PATH(context->lastPhotoTaken), O_CREAT | O_WRONLY);
            uint8_t readlen = (myCAM.getTotalLength() > 0 ? CAMERA_BUFF_LEN : 0);
            while(readlen > 0){
                digitalWrite(SD_CS, HIGH);
                readlen = myCAM.readBuff(buff, CAMERA_BUFF_LEN);
                photo.write(buff, readlen);
            }
            photo.flush();
            photo.close();
            #if DEBUG
            Serial.println("Photo Captured");
            #endif
        }
        myCAM.lowPowerOn();
        retVal = TASK_EXECUTION_OKAY;
    }


    return retVal;
}