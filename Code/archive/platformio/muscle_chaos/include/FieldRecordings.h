//////////////////// TSUNAMI BEAT-LINKED PLAYBACK /////////////////////
///////////////////////////////////////////////////////////////////////
#ifndef FIELDRECORDINGS_H
#define FIELDRECORDINGS_H

#include <Arduino.h>
#include <Globals.h>

class FieldRecordings
{
public:
    void tsunami_beat_playback(int instr, int current_beat_in);
};

#endif