// ------------------ variables for interrupt timers ------------------
#ifndef TIMING_H
#define TIMING_H

#include <Arduino.h>
#include <Globals.h>

class Timing
{
public:
    static IntervalTimer pinMonitor;  // reads pins every 1 ms
    static  IntervalTimer masterClock; // 1 bar
    static volatile int counts[Globals::numInputs];
    static volatile unsigned long lastPinActiveTime[Globals::numInputs];
    static volatile unsigned long firstPinActiveTime[Globals::numInputs];

    // ------------------ timing and rhythm tracking ------------------
    static int current_beat_pos; // always stores the current position in the beat

    static int tapInterval;
    static int current_BPM;

    // -------------------------- TIMERS ------------------------------
    static void samplePins();
    static void masterClockTimer();

    // ----------------------- timer counter --------------------------
    // volatile int bar_step; // 0-32

    static volatile unsigned long masterClockCount;
    static volatile unsigned long beatCount;
    static volatile int currentStep; // 0-32
    static int next_beatCount;   // will be reset when timer restarts
    static volatile boolean sendMidiClock;
};

#endif