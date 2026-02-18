#ifndef CAMERA_H
#define CAMERA_H

#include "pinDefinitions.h"
#include "taskHandles.h"

#define CAMERA_IMAGE_QUALITY CAM_IMAGE_MODE_WQXGA2
// #define CAMERA_IMAGE_QUALITY CAM_IMAGE_MODE_96X96


#define CAMERA_DIRECTORY String("Captures/")
#define CAMERA_POSITION CAMERA_DIRECTORY + "pos.txt"
#define CAMERA_PHOTO_FILE_PATH(photoNum) (CAMERA_DIRECTORY + "Photo-" + (photoNum) + ".jpg")
#define CAMERA_CONFIG "camera.cfg"
#define DEFAULT_CAMERA_ROLLOVER 8000
#define CAMERA_BUFF_LEN 254

extern CAMERA_TASK arducamMegaCameraContext;

TASK_RETURN_CODE_T initCamera(CAMERA_TASK* context);

TASK_RETURN_CODE_T tickCamera(CAMERA_TASK* context);

#endif