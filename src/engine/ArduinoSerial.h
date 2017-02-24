//
//  BYBArduino.h
//  SpikeRecorder
//
//  Created by Stanislav Mircic on 11/26/14.
//  Copyright (c) 2014 BYB. All rights reserved.
//


#ifndef BACKYARDBRAINS_ARDUINOSERIAL_H
#define BACKYARDBRAINS_ARDUINOSERIAL_H

#define SIZE_OF_CIRC_BUFFER 4024
#define SIZE_OF_MESSAGES_BUFFER 64
#define ESCAPE_SEQUENCE_LENGTH 6

#include <iostream>
#include <fcntl.h>
#include <list>
#include <string>


//just MAC classes
#ifdef __APPLE__
    #include <termios.h>
    #include <CoreFoundation/CoreFoundation.h>
    #include <IOKit/IOKitLib.h>
    #include <IOKit/serial/IOSerialKeys.h>
    #include <IOKit/IOBSD.h>
#elif __linux__
    #include <termios.h>
#elif _WIN32
    #include <windows.h>
#endif


#ifdef _WIN32
    #include "mingw.thread.h"
#else
    #include <thread>
#endif

namespace BackyardBrains {

    class ArduinoSerial {
    public:
        ArduinoSerial();
        int openPort(const char *portName);
        int readPort(char * buffer);
        int getNewSamples(int16_t * obuffer);
        int processDataIntoSamples(char * buffer, int size, int16_t * obuffer);
        std::list<std::string> list;
        void getAllPortsList();
        void closeSerial(void);
        void setNumberOfChannelsAndSamplingRate(int numberOfChannels, int samplingRate);
        int writeToPort(const void *ptr, int len);
        int maxSamplingRate();
        int maxNumberOfChannels();
        int numberOfChannels();
        const char * currentPortName();
        std::string errorString;
        bool portOpened();
        void sendEventMessage(int eventType);
        void sendPotentiometerMessage(uint8_t value);
        void checkAllPortsForArduino();
        void askForBoardType();
        //std::string firmwareVersion;
        //std::string hardwareVersion;
        std::string hardwareType;
    private:
        std::thread scanningThread;
        void scanPortsThreadFunction(ArduinoSerial * ref);
        
        char circularBuffer[SIZE_OF_CIRC_BUFFER];
        int cBufHead;
        int cBufTail;
        int fd;//file descriptor
        int _numberOfChannels;
        int _samplingRate;
        int serialCounter;

        bool returnTailForOneAndCheck();
        bool checkIfNextByteExist();
        bool areWeAtTheEndOfFrame();
        bool checkIfHaveWholeFrame();
        
        void testEscapeSequence(unsigned int newByte, int offset);
        void executeContentOfMessageBuffer(int offset);
        void executeOneMessage(std::string typeOfMessage, std::string valueOfMessage, int offsetin);
        //void executeOneMessage(std::string typeOfMessage, std::string valueOfMessage, int offsetin);
        //void executeContentOfMessageBuffer(int offset);
        int escapeSequenceDetectorIndex;
        bool weAreInsideEscapeSequence;
        char messagesBuffer[SIZE_OF_MESSAGES_BUFFER];//contains payload inside escape sequence
        int messageBufferIndex;
        unsigned int escapeSequence[ESCAPE_SEQUENCE_LENGTH];
        unsigned int endOfescapeSequence[ESCAPE_SEQUENCE_LENGTH];
        
        #ifdef __APPLE__
            void macos_ports(io_iterator_t  * PortIterator);

        #elif _WIN32
            HANDLE port_handle;
            COMMCONFIG port_cfg_orig;
            COMMCONFIG port_cfg;
        #endif
        std::string _portName;
        bool _portOpened;
        bool triedToConfigureAgain;
    };

}

#endif /* defined(__SpikeRecorder__BYBArduino__) */
