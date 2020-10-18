#include <Globals.h>
// #include <vector>
// #include <MIDI.h>
boolean Globals::printStrokes = true;
String Globals::output_string[Globals::numInputs];
IntervalTimer Globals::masterClock; // 1 bar


std::vector<int> Globals::pins = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};

// int swell_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

int Globals::current_eighth_count = 0; // overflows at current_beat_pos % 8
int Globals::current_16th_count = 0;   // overflows at current_beat_pos % 2
int Globals::last_eighth_count = 0;    // stores last eightNoteCount for comparison
int Globals::last_16th_count = 0;      // stores last eightNoteCount for comparison

int Globals::tapInterval = 500; // 0.5 s per beat for 120 BPM
int Globals::current_BPM = 120;

// ----------------------------- timer counter ---------------------------------
volatile unsigned long Globals::masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long Globals::beatCount = 0;
int Globals::next_beatCount = 0; // will be reset when timer restarts
volatile boolean Globals::sendMidiClock = false;

///////////////////// SET STRING FOR PLAY LOGGING /////////////////////
///////////////////////////////////////////////////////////////////////
void Globals::setInstrumentPrintString(DrumType drum, EffectsType effect)
{
    switch (effect)
    {
    case Monitor: // monitor: just print what is being played
        if (drum == Kick)
            Globals::output_string[drum] = "■\t"; // Kickdrum
        else if (drum == Cowbell)
            Globals::output_string[drum] = "▲\t"; // Crash
        else if (drum == Standtom1)
            Globals::output_string[drum] = "□\t"; // Standtom
        else if (drum == Standtom2)
            Globals::output_string[drum] = "O\t"; // Standtom
        else if (drum == Hihat)
            Globals::output_string[drum] = "x\t"; // Hi-Hat
        else if (drum == Tom1)
            Globals::output_string[drum] = "°\t"; // Tom 1
        else if (drum == Snare)
            Globals::output_string[drum] = "※\t"; // Snaredrum
        else if (drum == Tom2)
            Globals::output_string[drum] = "o\t"; // Tom 2
        else if (drum == Ride)
            Globals::output_string[drum] = "xx\t"; // Ride
        else if (drum == Crash1)
            Globals::output_string[drum] = "-X-\t"; // Crash
        else if (drum == Crash2)
            Globals::output_string[drum] = "-XX-\t"; // Crash
        break;

    case ToggleRhythmSlot: // toggle beat slot
        if (drum == Kick)
            Globals::output_string[drum] = "■\t"; // Kickdrum
        else if (drum == Cowbell)
            Globals::output_string[drum] = "▲\t"; // Crash
        else if (drum == Standtom1)
            Globals::output_string[drum] = "□\t"; // Standtom
        else if (drum == Standtom2)
            Globals::output_string[drum] = "O\t"; // Standtom
        else if (drum == Hihat)
            Globals::output_string[drum] = "x\t"; // Hi-Hat
        else if (drum == Tom1)
            Globals::output_string[drum] = "°\t"; // Tom 1
        else if (drum == Snare)
            Globals::output_string[drum] = "※\t"; // Snaredrum
        else if (drum == Tom2)
            Globals::output_string[drum] = "o\t"; // Tom 2
        else if (drum == Ride)
            Globals::output_string[drum] = "xx\t"; // Ride
        else if (drum == Crash1)
            Globals::output_string[drum] = "-X-\t"; // Crash
        else if (drum == Crash2)
            Globals::output_string[drum] = "-XX-\t"; // Crash
        break;

    case FootSwitchLooper: // add an ! if pinAction == 3 (replay logged rhythm)
        if (drum == Kick)
            Globals::output_string[drum] = "!■\t"; // Kickdrum
        else if (drum == Cowbell)
            Globals::output_string[drum] = "!▲\t"; // Crash
        else if (drum == Standtom1)
            Globals::output_string[drum] = "!□\t"; // Standtom
        else if (drum == Standtom2)
            Globals::output_string[drum] = "!O\t"; // Standtom
        else if (drum == Hihat)
            Globals::output_string[drum] = "!x\t"; // Hi-Hat
        else if (drum == Tom1)
            Globals::output_string[drum] = "!°\t"; // Tom 1
        else if (drum == Snare)
            Globals::output_string[drum] = "!※\t"; // Snaredrum
        else if (drum == Tom2)
            Globals::output_string[drum] = "!o\t"; // Tom 2
        else if (drum == Ride)
            Globals::output_string[drum] = "!xx\t"; // Ride
        else if (drum == Crash1)
            Globals::output_string[drum] = "!-X-\t"; // Crash
        else if (drum == Crash2)
            Globals::output_string[drum] = "!-XX-\t"; // Crash
        break;

        // case 5: // print swell_val for repeated MIDI notes in "swell" mode
        //   Globals::output_string[incoming_i] = swell_val[incoming_i];
        //   Globals::output_string[incoming_i] += "\t";
        //   break;
    }
}

// --------------------------------------------------------------------

// general pin reading:
// static int pinValue(Instrument *instrument)
// {
//     return abs(instrument->sensitivity.noiseFloor - analogRead(instrument->pin));
// }
// --------------------------------------------------------------------
/* --------------------------------------------------------------------- */
/* -------------------------- TIMED INTERRUPTS ------------------------- */
/* --------------------------------------------------------------------- */

// void Globals::samplePins()
// {
//     // ------------------------- read all pins -----------------------------------
//     for (Instrument *instrument : instruments)
//     {
//         if (pinValue(instrument) > instrument->sensitivity.threshold)
//         {
//             if (instrument->timing.counts < 1)
//                 instrument->timing.firstPinActiveTime = millis();
//             instrument->timing.lastPinActiveTime = millis();
//             instrument->timing.counts++;
//         }
//     }
// }
// -----------------------------------------------------------------------------

void Globals::masterClockTimer()
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
}
// ---------------------------------------------------------------------------