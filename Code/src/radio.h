#ifndef RADIO_H
#define RADIO_H

#include <Arduino.h>
#include <SD.h>

// Radio state machine states
typedef enum
{
    init_st,
    sleep_st,
    tx_st,
    rx_st,
} radio_st;

// Public interface
void radio_init(); // Initialize radio module
void radio_loop(); // Main radio state machine loop

#endif // RADIO_H