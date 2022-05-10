#pragma once
#include <vector>
#include <Score/Score.h>

class Instrument;

class JSON
{
public:
    static void compose_and_send_json(std::vector<Instrument *> instruments);
};

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

    static boolean newData;

    static void loop();
    static void recvWithStartEndMarkers();
    static void parseData();
    static void showParsedData();
};