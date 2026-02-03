#include "radio.h"

#include "camera.h"

#include <LoRa.h>

#define SS 3                // CS pin for RF module
#define RESET 1             // Reset for RF module
#define DIO_0 -1            // We will not have this pin so this will disable it
#define TX_POWER 15         // Max input power for FEM (SKY66122-11) is 16dBm
#define SPREADING_FACTOR 12 // set the spreading factor to 12 to maximize distance
#define FREQ 915E6

#define TXRX 2 // CTX pin for Front End Module
#define FEM 0  // Enable disable pin for Front End Module

#define TRANSMIT_REFRESH_DELAY 6000 // time between transmits in ms
#define RECEIVE_REFRESH_DELAY 10000  // time spent in receive in ms

#define PACKET_SIZE 255 // Size of transmitted packet, max is 255 bytes

class RadioManager
{
private:
    bool isFunctional = false;

    radio_st radio_current_st = init_st;

    boolean txComplete = false;
    byte txBuffer[PACKET_SIZE];

    int photoNumber = 0;
    File imageFile;

    unsigned long timeSinceLastTransition = 0;

    // Radio state machine transitions
    void stateTransition()
    {
        // State transition switch case
        switch (radio_current_st)
        {
        case init_st:
            // State machine init stuff
            mode_sleep();
            radio_current_st = sleep_st;
            break;
        case sleep_st:
            // Will move to tx if enough time has elapsed else stay in sleep
            if (millis() - timeSinceLastTransition >= TRANSMIT_REFRESH_DELAY || millis() < timeSinceLastTransition)
            {
                mode_tx();

                timeSinceLastTransition = millis();
                radio_current_st = tx_st;
            }
            break;
        case tx_st:
            // Will transition to RX after transmission is complete
            if (txComplete)
            { // flag set in tx function
                mode_rx();
                radio_current_st = rx_st;
            }
            break;
        case rx_st:
            // Call RX transition function, will wait in RX for a certain amount of time, then transition
            if (millis() - timeSinceLastTransition >= RECEIVE_REFRESH_DELAY || millis() < timeSinceLastTransition)
            {
                mode_sleep();
                timeSinceLastTransition = millis();
                radio_current_st = sleep_st;
            }
            break;
        default:
#ifdef DEBUG
            Serial.println("Undefined state transition!");
#endif
            break;
        }
    }

    // Run radio state machine actions
    void stateAction()
    {
        // State action switch case
        switch (radio_current_st)
        {
        case init_st:
            // Doesn't need any actions, already all called in setup
            break;
        case sleep_st:
            // No needed sleep actions all will be in transition actions
            break;
        case tx_st:
            {
                // Do transmit things
                #if DEBUG
                Serial.println("Beginning transmission");
                #endif

                // No file has been pulled
                if (imageFile == NULL)
                {
                    // open the position file and read in the current position file
                    #if DEBUG
                    Serial.println("Opening image");
                    #endif
                    File positionReader = SD.open(CAMERA_POSITION, FILE_READ);
                    positionReader.seek(0);
                    photoNumber = positionReader.parseInt();
                    positionReader.close();

                    // Pull the file to transmit
                    imageFile = SD.open(CAMERA_DIRECTORY + String(photoNumber) + ".jpg", FILE_READ);
                    imageFile.seek(0);
                }

                // Read data from file one at a time to find EOF
                #if DEBUG
                Serial.println("Reading packet");
                #endif
                int currentPacketSize = PACKET_SIZE;
                for (int i = 0; i < PACKET_SIZE; i++)
                {
                    // Put file data in the buffer
                    txBuffer[i] = imageFile.read();

                    // Detect EOF
                    if (txBuffer[i] == -1)
                    {
                        currentPacketSize = i;
                        imageFile.close();
                        break;
                    }
                }

                // Transmit the buffer
                #if DEBUG
                Serial.println("Sending data to radio");
                #endif
                LoRa.beginPacket();
                #if DEBUG
                Serial.println("Begun");
                #endif
                LoRa.write(txBuffer, currentPacketSize);
                 #if DEBUG
                Serial.println("Written");
                #endif
                LoRa.endPacket();
                #if DEBUG
                Serial.println("Tx complete");
                #endif

                break;
            }
        case rx_st:
            // Do receive things

#ifdef DEBUG
            // print RSSI of packet
            Serial.print("' with RSSI ");
            Serial.println(LoRa.packetRssi());
#endif
            break;
        default:
#ifdef DEBUG
            Serial.println("Undefined state action!");
#endif
            break;
        }
    }

    // Do all the actions to put the radio to sleep
    void mode_sleep()
    {
        // Sleep mode actions
        digitalWrite(FEM, LOW);  // Turn FEM off
        digitalWrite(TXRX, LOW); // Set tx\rx low

        LoRa.sleep();
    }

    // Do all the actions to put the radio in RX
    void mode_rx()
    {
        // Receive mode setup
        digitalWrite(FEM, HIGH); // Turn FEM on
        digitalWrite(TXRX, LOW); // Set tx\rx low for rx mode

        LoRa.receive();
    }

    // Do all the actions to put the radio in TX
    void mode_tx()
    {
        // Transmit mode setup
        digitalWrite(FEM, HIGH);  // Turn FEM on
        digitalWrite(TXRX, HIGH); // Set tx\rx high for tx mode
    }

public:
    // init stuff here
    void radio_init()
    {
        // init LoRa Library
        // LoRa.setSPI(spi); // Set SPI if not default
        #if DEBUG
        Serial.println("Starting LoRa");
        #endif
        LoRa.setPins(SS, RESET, DIO_0);
        if (!LoRa.begin(FREQ))
        {
#ifdef DEBUG
            Serial.println("Starting LoRa failed!");
#endif
        }
        else{
            isFunctional = true;

            #if DEBUG
            Serial.println("Setting TX power");
            #endif
            LoRa.setTxPower(TX_POWER);
            LoRa.setSpreadingFactor(SPREADING_FACTOR);
            LoRa.enableCrc(); // enable Cyclic Redundancy Checks to increase successful packets

            // Wont have a callback
            // LoRa.onReceive(onReceive); // Set up callback for receiving

            // Setup FEM pins
            pinMode(FEM, OUTPUT);
            pinMode(TXRX, OUTPUT);

            radio_current_st = init_st;
        }
    }

    // Loop function that runs radio state machine
    void radio_loop()
    {
        if(isFunctional){
            stateTransition();

            stateAction();
        }
        else{
            if(millis() % 1024 == 0){
                radio_init();
            }
        }
    }
};
