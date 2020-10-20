#include <Globals.h>
#include <Arduino.h>

int Core::pinValue(int pinVal_pointer_in)
{
  return abs(Calibration::noiseFloor[pinVal_pointer_in] - analogRead(Globals::pins[pinVal_pointer_in]));
}

// Globals::numInputs = 7;
uint8_t Globals::leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};
uint8_t Globals::pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
uint8_t Globals::pinAction[] = {8, 4, 8, 8, 8, 8, 1, 0}; // array to be changed within code loop.
/*
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with MIDI playback and volume changet
*/

// ------------------------- Debug variables --------------------------
boolean Debug::use_responsiveCalibration = false;
boolean Debug::printStrokes = true;
boolean Debug::printNormalizedValues_ = false;
boolean Debug::do_print_to_console = true;
boolean Debug::do_send_to_processing = false;

String DrumtypeToHumanreadable(DrumType type)
{
  switch (type)
  {
  case Snare:
    return "Snare";
  case Hihat:
    return "Hihat";
  case Kick:
    return "Kick";
  case Tom1:
    return "Tom1";
  case Tom2:
    return "Tom2";
  case Standtom1:
    return "Standtom1";
  case Standtom2:
    return "Standtom2";
  case Ride:
    return "Ride";
  case Crash1:
    return "Crash1";
  case Crash2:
    return "Crash2";
  case Cowbell:
    return "Cowbell";
  }
  return "";
}

// ------------------ timing and rhythm tracking ------------------
int Timing::current_beat_pos = 0;

int Timing::tapInterval = 500; // 0.5 s per beat for 120 BPM
int Timing::current_BPM = 120;

int Calibration::calibration[Globals::numInputs][2]; // [instrument][0:threshold, 1:min_counts_for_signature], will be set in setup()
int Calibration::noiseFloor[Globals::numInputs];
int Calibration::globalDelayAfterStroke = 10; // 50 ms TODO: assess best timing for each instrument

///////////////////////////////// HARDWARE ////////////////////////////
///////////////////////////////////////////////////////////////////////
boolean Hardware::footswitch_is_pressed = false;
void footswitch_pressed()
{
  switch (Hardware::FOOTSWITCH_MODE)
  {
  case (Hardware::LOG_BEATS):
    // set pinMode of all instruments to 3 (record what is being played)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      Swell::lastPinAction[i] = Globals::pinAction[i]; // TODO: lastPinAction seems to be overwritten quickly, so it will not be able to return to its former state. fix this!
      Globals::pinAction[i] = 3;                       // TODO: not for Cowbell?
      for (int j = 0; j < 8; j++)
        Score::set_rhythm_slot[i][j] = false; // reset entire record
    }
    break;

  case (Hardware::HOLD_CC): // prevents swell_vals to be changed in swell_beat()
    Hardware::footswitch_is_pressed = true;
    break;

  case (Hardware::RESET_TOPO): // resets beat_topography (for all instruments)
    for (int i = 0; i < Globals::numInputs; i++)
    {
      // reset 8th-note-topography:
      for (int j = 0; j < 8; j++)
      {
        Score::beat_topography_8.array[i][j] = 0;
      }

      // reset 16th-note-topography:
      for (int j = 0; j < 16; j++)
      {
        Score::beat_topography_16.array[i][j] = 0;
      }
    }

    break;

  default:
    Serial.println("Footswitch mode not defined!");
    break;
  }
}
// --------------------------------------------------------------------

void footswitch_released()
{
  switch (Hardware::FOOTSWITCH_MODE)
  {
  case (Hardware::LOG_BEATS):
    for (int i = 0; i < Globals::numInputs; i++)
      Globals::pinAction[i] = Score::initialPinAction[i];
    break;

  case (Hardware::HOLD_CC):
    Hardware::footswitch_is_pressed = false;
    break;

  default:
    break;
  }
}

// --------------------------------------------------------------------

int Score::notes_list[] = {60, 61, 45, 74, 72, 44, 71};
int Score::cc_chan[] = {50, 0, 0, 25, 71, 50, 0, 0}; // needed in pinAction 5 and 6

/* channels on mKORG:
   44=cutoff
   71=resonance
   50=amplevel
   23=attack
   25=sustain
   26=release
*/

int Score::allocated_channels[] = {0, 0, 0, 0, 0, 0, 0}; // channels to send audio from tsunami to

// hard-coded list of BPMs of tracks stored on Tsunami's SD card.
// TODO: somehow make BPM accessible from file title
float Score::track_bpm[256] =
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

/* --------------------------------------------------------------------- */
/* -------------------------- TIMED INTERRUPTS ------------------------- */
/* --------------------------------------------------------------------- */

void Timing::samplePins()
{
  // ------------------------- read all pins -----------------------------------
  for (int pinNum = 0; pinNum < Globals::numInputs; pinNum++)
  {
    if (Core::pinValue(pinNum) > Calibration::calibration[pinNum][0])
    {
      if (Timing::counts[pinNum] < 1)
        Timing::firstPinActiveTime[pinNum] = millis();
      Timing::lastPinActiveTime[pinNum] = millis();
      Timing::counts[pinNum]++;
    }
  }
}

// ----------------------------- timer counter ---------------------------------
volatile unsigned long Timing::masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long Timing::beatCount = 0;
// volatile int bar_step; // 0-32
volatile int Timing::currentStep; // 0-32
int Timing::next_beatCount = 0;   // will be reset when timer restarts
volatile boolean Timing::sendMidiClock = false;

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

  // ---------------------------------------------------------------------------
}
