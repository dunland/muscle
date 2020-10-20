#ifndef SWELL_H
#define SWELL_H


#include <Arduino.h>
#include <Globals.h>


class Swell
{
public:
    int swell_val[Globals::numInputs];
    boolean lastPinAction[Globals::numInputs];

    int num_of_swell_taps[Globals::numInputs];     // will be used in both rec() and perform(). serves as swell_val for MIDI notes.
    int swell_stroke_interval[Globals::numInputs]; // will be needed for timed replay
    int swell_state[Globals::numInputs];
    unsigned long swell_beatPos_sum[Globals::numInputs];
    int swell_beatStep[Globals::numInputs]; // increases with beatCount and initiates action.

    // FUNCTIONS:
    void init();
    void rec(int instr); // remembers beat stroke position
    void perform(int instr, int perform_action); // should be run once a 32nd-beat-step
};

#endif