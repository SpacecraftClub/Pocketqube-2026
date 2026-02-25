#ifndef PIN_DEFINITIONS_H
#define PIN_DEFINITIONS_H
#include <SdFat.h>


//legacy board for prototyping
#define BOARD_PRE_2025 false



#define RAD_CS 3
#define MS5611_I2C_ADDR 0x76
#define MMC5603_I2C_ADDR 0x30




#if BOARD_PRE_2025
#define SD_CS 7
#define CAM_CS 2
#else
#define SD_CS 6
#define CAM_CS 7
#endif


#define SD_CONFIG SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(10))
extern SdFs SD_CARD;

#endif