// ------------------ timing and rhythm tracking ------------------
#include <Globals.h>
#include <Arduino.h>
#include <Timing.h>

IntervalTimer Timing::pinMonitor;  // reads pins every 1 ms
IntervalTimer Timing::masterClock; // 1 bar
volatile int Timing::counts[Globals::numInputs];
volatile unsigned long Timing::lastPinActiveTime[Globals::numInputs];
volatile unsigned long Timing::firstPinActiveTime[Globals::numInputs];

// ------------------ timing and rhythm tracking ------------------
int Timing::current_beat_pos = 0; // always stores the current position in the beat

int Timing::tapInterval = 500; // 0.5 s per beat for 120 BPM
int Timing::current_BPM = 120;

// ----------------------- timer counter --------------------------
// volatile int bar_step; // 0-32

volatile unsigned long Timing::masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long Timing::beatCount = 0;
volatile int Timing::currentStep; // 0-32
int Timing::next_beatCount = 0;   // will be reset when timer restarts
volatile boolean Timing::sendMidiClock = false;

// ------------------------------- TIMERS -----------------------------
void Timing::samplePins()
{
    // ------------------------- read all pins -----------------------------------
    for (int pinNum = 0; pinNum < Globals::numInputs; pinNum++)
    {
        if (Core::pinValue(pinNum, Calibration::noiseFloor) > Calibration::calibration[pinNum][0])
        {
            if (Timing::counts[pinNum] < 1)
                Timing::firstPinActiveTime[pinNum] = millis();
            Timing::lastPinActiveTime[pinNum] = millis();
            Timing::counts[pinNum]++;
        }
    }
}

// ------------------------- timer counter ----------------------------
void Timing::masterClockTimer()
{
    /*
    timing              fraction          bar @ 120 BPM    bar @ 180 BPM
    1 bar             = 1               = 2 s       | 1.3 s
    1 beatCount       = 1/32 bar        = 62.5 ms   | 41.7 ms
    stroke precision  = 1/4 beatCount   = 15.625 ms | 10.4166 ms


    |     .-.
    |    /   \         .-.
    |   /     \       /   \       .-.     .-.     _   _
    +--/-------\-----/-----\-----/---\---/---\---/-\-/-\/\/---
    | /         \   /       \   /     '-'     '-'
    |/           '-'         '-'
    |--------2 oscil---------|
    |------snare = 10 ms-----|

  */

    masterClockCount++; // will rise infinitely

    // ------------ 1/32 increase in 4 full precisionCounts -----------
    if (masterClockCount % 4 == 0)
    {
        beatCount++; // will rise infinitely
    }

    // evaluate current position of beat in bar for stroke precision
    // 2020-09-07: this doesn't help and it's also not working...
    // if ((masterClockCount % 4) >= next_beatCount - 2)
    // {
    //   currentStep = next_beatCount;
    //   next_beatCount += 4;
    // }

    // prepare MIDI clock:
    sendMidiClock = (((masterClockCount % 4) / 3) % 4 == 0);

    // ----------------------------------------------------------------
}
