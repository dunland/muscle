#include <Globals.h>
#include <Tsunami.h>
// #include <vector>
// #include <MIDI.h>

std::vector<int> Globals::pins = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
std::vector<int> Globals::leds = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};

boolean Globals::printStrokes = true;
String Globals::output_string[Globals::numInputs];
boolean Globals::use_responsiveCalibration = false;
boolean Globals::printNormalizedValues_ = false;
boolean Globals::do_print_to_console = true;
boolean Globals::do_send_to_processing = false;

IntervalTimer Globals::masterClock; // 1 bar

Tsunami Globals::tsunami;
float Globals::track_bpm[256] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
        1, 1, 90, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 73, 1, 1, 1,
        100, 1, 1, 1, 200, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 41, 1, 1,
        103, 1, 1, 1, 1, 1, 1, 93, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 78, 1, 100, 100, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 100, 1, 1, 1, 1,
        1, 1, 1, 1, 100, 60};

// int swell_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

int Globals::current_beat_pos = 0;     // always stores the current position in the beat
int Globals::current_eighth_count = 0; // overflows at current_beat_pos % 8
int Globals::current_16th_count = 0;   // overflows at current_beat_pos % 2
int Globals::last_eighth_count = 0;    // stores last eightNoteCount for comparison
int Globals::last_16th_count = 0;      // stores last eightNoteCount for comparison

int Globals::tapInterval = 500; // 0.5 s per beat for 120 BPM
int Globals::current_BPM = 120;

boolean Globals::footswitch_is_pressed = false;

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

<<<<<<< HEAD
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

=======
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
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

<<<<<<< HEAD
void Globals::print_to_console(String message_to_print)
{
  if (Globals::do_print_to_console)
    Serial.print(message_to_print);
=======
// ----------------------------- TOPOGRAPHIES -------------------------
void Globals::derive_topography(TOPOGRAPHY *original, TOPOGRAPHY *abstraction)
{
    for (int i = 0; i < 16; i++)
    {
        if (original->a_16[i] == 0 && original->a_16_prior[i] == 0) // empty slot repeatedly not played
        {
            abstraction->a_16[i]++;
        }
        else if (original->a_16[i] > 0 && original->a_16[i] > original->a_16_prior[i]) // occupied slot repeatedly played
        {
            abstraction->a_16[i]++;
        }
        original->a_16_prior = original->a_16;
    }
}

// ---------------- smoothen 16-bit array using struct ----------------
void Globals::smoothen_dataArray(TOPOGRAPHY *topography)
{
    /* input an array of size 16
1. count entries and create squared sum of each entry
2. calculate (squared) fraction of total for each entry
3. get highest of these fractions
4. get ratio of highest fraction to other and reset values if ratio > threshold
->  
*/

    // int len = *(&topography.a_16 + 1) - topography.a_16;
    int len = topography->a_16.size(); // TODO: use dynamic vector topography.a instead
    int entries = 0;
    int squared_sum = 0;
    topography->regular_sum = 0;

    // count entries and create squared sum:
    for (int j = 0; j < len; j++)
    {
        if (topography->a_16[j] > 0)
        {
            entries++;
            squared_sum += topography->a_16[j] * topography->a_16[j];
            topography->regular_sum += topography->a_16[j];
        }
    }

    topography->regular_sum = topography->regular_sum / entries;

    // calculate site-specific (squared) fractions of total:
    float squared_frac[len];
    for (int j = 0; j < len; j++)
        squared_frac[j] =
            float(topography->a_16[j]) / float(squared_sum);

    // get highest frac:
    float highest_squared_frac = 0;
    for (int j = 0; j < len; j++)
        highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

    // get "topography height":
    // divide highest with other entries and reset entries if ratio > threshold:
    for (int j = 0; j < len; j++)
        if (squared_frac[j] > 0)
            if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > topography->threshold)
            {
                topography->a_16[j] = 0;
                entries -= 1;
            }

    topography->average_smooth = 0;
    // assess average topo sum for loudness
    for (int j = 0; j < 8; j++)
        topography->average_smooth += topography->a_16[j];
    topography->average_smooth = int((float(topography->average_smooth) / float(entries)) + 0.5);
}

// ---------------------------- DEBUG FUNCTIONS ------------------------------
void Globals::print_to_console(String message_to_print)
{
    if (Globals::do_print_to_console)
        Serial.print(message_to_print);
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
}

void Globals::println_to_console(String message_to_print)
{
<<<<<<< HEAD
  if (Globals::do_print_to_console)
    Serial.println(message_to_print);
=======
    if (Globals::do_print_to_console)
        Serial.println(message_to_print);
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
}

void Globals::send_to_processing(int message_to_send)
{
<<<<<<< HEAD
  if (Globals::do_send_to_processing)
    Serial.write(message_to_send);
=======
    if (Globals::do_send_to_processing)
        Serial.write(message_to_send);
>>>>>>> b7acb17d225ad8ec1a0afd56cafcd5e07798be9e
}