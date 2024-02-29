#pragma once
#include <vector>
#include <Song.h>
#include <Instruments.h>

class Instrument;

class NanoKontrol
{
public:
    static const byte numChars = 32;
    static char receivedChars[numChars];
    static char tempChars[numChars]; // temporary array for use when parsing

    // variables to hold the parsed data
    static char address1[numChars];
    static char address2[numChars];
    static int incomingInt;
    static int incomingChannel, incomingValue;

    static boolean newData;

    static Instrument *instrument;
    static Instrument::MIDI_TARGET *midiTarget;
    static const int SET_INSTRUMENT = 0; // select instrument
    static const int SET_MIDI_TARGET = 0; // select instrument's midiTarget
    static const int SET_CC_TYPE = 0;
    static const int SET_DEST = 1; // midi device destination
    static const int SET_CC_MIN = 0;
    static const int SET_INCREASE = 1;
    static const int SET_CC_MAX = 0;
    static const int SET_DECREASE = 1;
    static int paramsToChange[4];

    static void loop();
    static void recvWithStartEndMarkers();
    static void parseData();
    static void allocateData();
    static void printToLCD();
    static void printToSerial();
};