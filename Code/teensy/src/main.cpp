/*
   ------------------------------------
   November 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------
  0. calibrate sensors
  1. set calibration[instrument][0/1] values. (in setup())
    0:threshold, 1:min num of threshold crossings
  2. set mode for instruments:

    pinActions:
    -----------
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with MIDI playback and volume change
*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
#include <Arduino.h>
#include <vector>
#include <MIDI.h>
#include <Tsunami.h>
#include <Globals.h>
#include <Instruments.h>
#include <Effects.h>
#include <Hardware.h>
#include <Score.h>

midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial &)Serial2); // same as MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

Instrument *snare;
Instrument *hihat;
Instrument *kick;
// Instrument tom1;
Instrument *tom2;
Instrument *standtom;
Instrument *crash1;
Instrument *cowbell;
// Instrument crash2;
// Instrument ride;

static std::vector<Instrument *> instruments;

// ------------------------- interrupt timers -------------------------
IntervalTimer pinMonitor; // reads pins every 1 ms

// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------

Score score1;

// TOPOGRAPHY regularity;

/*
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with ControlChange of instrument-specific MIDI CC
    9 = PlayMidi_rawPin: instead of stroke detection, MIDI notes are sent directly when sensitivity threshold is crossed. may sound nice on cymbals..
    10 = CC_Effect_rawPin: instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
*/
// int initialPinAction[Globals::numInputs];
// int allocated_track[Globals::numInputs];                   // tracks will be allocated in tsunami_beat_playback
// int allocated_channels[] = {0, 0, 0, 0, 0, 0, 0}; // channels to send audio from tsunami to

/////////////////////////// general pin reading ///////////////////////
///////////////////////////////////////////////////////////////////////

int pinValue(Instrument *instrument)
{
  return abs(instrument->sensitivity.noiseFloor - analogRead(instrument->pin));
}
// --------------------------------------------------------------------

//////////////////////////// PRINT NORMALIZED VALUES //////////////////
///////////////////////////////////////////////////////////////////////
void printNormalizedValues(boolean printNorm_criterion)
{
  // useful debugger for column-wise output of raw/normalised values:

  if (printNorm_criterion == true)
  {
    static unsigned long lastMillis;
    if (millis() != lastMillis)
    {
      for (auto &instrument : instruments)
      {
        // static int countsCopy[Globals::numInputs];
        //noInterrupts();
        //countsCopy[i] = counts[i];
        //interrupts();
        //Globals::print_to_console(pins[i]);
        //Globals::print_to_console(":\t");
        Globals::print_to_console(pinValue(instrument));
        Globals::print_to_console("\t");
        //Globals::print_to_console(", ");
        //Globals::print_to_console(countsCopy[i]);
      }
      Globals::println_to_console("");
    }
    lastMillis = millis();
  }
}
// --------------------------------------------------------------------

// interrupt for sensor reading ---------------------------------------
void samplePins()
{
  // read all pins:
  for (auto &instrument : instruments)
  // Using a for loop with iterator
  {
    if (pinValue(instrument) > instrument->sensitivity.threshold)
    {
      if (instrument->timing.counts < 1)
        instrument->timing.firstPinActiveTime = millis();
      instrument->timing.lastPinActiveTime = millis();
      instrument->timing.counts++;
    }
  }
}

// --------------------------------------------------------------------

/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup()
{

  Globals::do_print_to_console = true;
  Globals::do_send_to_processing = false;

  Serial.begin(115200);
  // Serial3.begin(57600); // contained in tsunami.begin()
  while (!Serial)
    ; // prevents Serial flow from just stopping at some (early) point.
  // delay(1000); // alternative to line above, if run with external power (no computer)

  MIDI.begin(MIDI_CHANNEL_OMNI);

  delay(1000);              // wait for Tsunami to finish reset // redundant?
  Globals::tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
  delay(100);
  Globals::tsunami.stopAllTracks(); // in case Tsunami was already playing.
  Globals::tsunami.samplerateOffset(0, 0);
  Globals::tsunami.setReporting(true); // Enable track reporting from the Tsunami
  delay(100);                          // some time for Tsunami to respond with version string

  //------------------------ initialize pins --------------------------
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);

  // setup names of elements for Serial communication (to processing): -------------------
  Effect::beat_sum.tag = "v";

  // ------------------------ INSTRUMENT SETUP ------------------------
  // instantiate instruments:
  snare = new Instrument(A3, Snare);
  hihat = new Instrument(A4, Hihat);
  kick = new Instrument(A0, Kick);
  tom2 = new Instrument(A5, Tom2);
  standtom = new Instrument(A1, Standtom1);
  cowbell = new Instrument(A2, Cowbell);
  crash1 = new Instrument(A6, Crash1);

  instruments = {snare, hihat, kick, tom2, standtom, cowbell, crash1};

  // initialize arrays:
  for (auto &instrument : instruments)
  {
    pinMode(instrument->led, OUTPUT);
    for (int j = 0; j < 8; j++)
    {
      instrument->score.read_rhythm_slot[j] = false;
      instrument->score.set_rhythm_slot[j] = false;
      instrument->topography.a_8[j] = 0;
      instrument->topography.a_16[j] = 0;
      instrument->topography.a_16[j * 2] = 0;
    }
  }

  // set instrument calibration array
  // values as of 2020-08-27:

  // tom1->sensitivity.threshold = 200;
  // tom1->sensitivity.crossings = 20;
  snare->setup_sensitivity(180, 12, 10, false);
  hihat->setup_sensitivity(80, 15, 10, false);
  standtom->setup_sensitivity(70, 30, 10, false); // 60, 20
  tom2->setup_sensitivity(300, 9, 10, false);     // 300, 18
  kick->setup_sensitivity(100, 16, 10, false);
  cowbell->setup_sensitivity(80, 15, 10, false);
  crash1->setup_sensitivity(300, 2, 5, false);
  snare->setup_sensitivity(180, 12, 10, false);

  // calculate noise floor:
  for (auto &instrument : instruments)
    instrument->calculateNoiseFloor(instrument);

  Globals::println_to_console("\n..calculating noiseFloor done.");

  Globals::println_to_console("assigning effects...");

  // assign effects to instruments:
  snare->effect = CC_Effect_rawPin;
  hihat->effect = TapTempo;
  kick->effect = CC_Effect_rawPin;
  // tom1->effect = CC_Effect_rawPin;
  tom2->effect = Monitor;
  standtom->effect = CC_Effect_rawPin;
  cowbell->effect = CC_Effect_rawPin;
  crash1->effect = CC_Effect_rawPin;
  // instruments[Ride]->effect = Monitor;

  // ---------------------------- SCORE -------------------------------
  Globals::println_to_console("setting up variables for score..");
  Globals::print_to_console("note seed for score = ");
  randomSeed(analogRead(A0) * analogRead(A19));
  score1.notes.push_back(int(random(24, 36)));
  Globals::println_to_console(score1.notes[0]);

  snare->midi.active_note = score1.notes[0] + 12 + 4;
  hihat->midi.active_note = score1.notes[0] + 24;
  kick->midi.active_note = score1.notes[0];
  // tom1->midi.active_note = 71;
  tom2->midi.active_note = 74;
  standtom->midi.active_note = score1.notes[0] + 3;
  cowbell->midi.active_note = 60;
  crash1->midi.active_note = 74;

  Globals::println_to_console("setting up midi channels..");
  // midi channels (do not use any Type twice → smaller/bigger will be ignored..)
  snare->setup_midi(DelayTime, 127, 30, 10, 0.1);
  hihat->setup_midi(None, 127, 30, 10, 0.1);
  kick->setup_midi(Release, 127, 0, 50, 0.2);
  // tom1->setup_midi(Sustain, 127, 0, 5, 0.01);
  tom2->setup_midi(Resonance, 127, 30, 1, 0.01);
  standtom->setup_midi(Sustain, 127, 40, 15, 0.01);
  cowbell->setup_midi(DelayDepth, 90, 0, 30, 0.1);
  crash1->setup_midi(Cutoff, 127, 40, 4, 0.1);

  // print startup information:
  Globals::println_to_console("-----------------------------------------------");
  Globals::println_to_console("calibration values set as follows:");
  Globals::println_to_console("instr\tthrshld\tcrosses\tnoiseFloor");
  for (int i = 0; i < instruments.size(); i++)
  {
    Globals::print_to_console(Globals::DrumtypeToHumanreadable(DrumType(i)));
    Globals::print_to_console("\t");
    Globals::print_to_console(instruments[i]->sensitivity.threshold);
    Globals::print_to_console("\t");
    Globals::print_to_console(instruments[i]->sensitivity.crossings);
    Globals::print_to_console("\t");
    Globals::println_to_console(instruments[i]->sensitivity.noiseFloor);
  }
  Globals::println_to_console("-----------------------------------------------");

  // -------------------------- START TIMERS --------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond

  Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel

  // send MIDI-RealTime-Start-Message:
  // MIDI.sendRealTime(midi::Start);
}

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{
  Globals::tsunami.update(); // keeps variables for playing tracks etc up to date

  // ------------------------- DEBUG AREA -----------------------------
  printNormalizedValues(false);

  // --------------------- INCOMING SIGNALS FROM PIEZOS ---------------
  // (define what should happen when instruments are hit)
  for (auto &instrument : instruments)
  {
    // if (instrument->effect == PlayMidi_rawPin || instrument->effect == CC_Effect_rawPin)
    //  instrument->trigger(instrument, MIDI);

    if (instrument->stroke_detected()) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      instrument->trigger(instrument, MIDI); // runs trigger function according to instrument's EffectType
      // send instrument stroke to processing:
      // Globals::send_to_processing('i');
    }
  }

  ///////////////////////////// TIMED ACTIONS /////////////////////////
  // (automatically invoke rhythm-linked actions)
  static int last_beat_pos = 0;
  static boolean toggleLED = true;
  static boolean sendMidiCopy = false;

  // get current beat position:
  noInterrupts();
  Globals::current_beat_pos = Globals::beatCount % 32; // (beatCount increases infinitely)
  sendMidiCopy = Globals::sendMidiClock;               // MIDI flag for Clock to be sent
  interrupts();

  // send MIDI-Clock on beat:
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */
  if (sendMidiCopy)
  {
    Serial2.write(0xF8); // MIDI-clock has to be sent 24 times per quarter note
    noInterrupts();
    Globals::sendMidiClock = false;
    interrupts();
  }

  //----------------------- DO THINGS ONCE PER 32nd-step:--------------
  //-------------------------------------------------------------------

  if (Globals::current_beat_pos != last_beat_pos)
  {
    // tidy up with previous beat position ----------------------------
    // apply topography derivations from previous beats
    // → problem: if there was any stroke at all, it was probably not on the very first run BEFORE derivation was executed

    // -------------------------- 32nd-notes --------------------------

    // print millis and current beat:
    if (Globals::do_send_to_processing)
      Globals::print_to_console("m");
    Globals::print_to_console(String(millis()));
    Globals::print_to_console("\t");
    // Globals::print_to_console(Globals::current_eighth_count + 1); // if you want to print 8th-steps only
    if (Globals::do_send_to_processing)
      Globals::print_to_console("b");
    Globals::print_to_console(Globals::current_beat_pos);
    Globals::print_to_console("\t");

    // -------------------------- full notes: -------------------------
    if (Globals::current_beat_pos == 0)
    {
      Globals::print_to_console("score_state = ");
      Globals::println_to_console(Score::score_state);
      // MIDI.sendRealTime(midi::Continue);
    }

    // ------------------------- quarter notes: -----------------------
    if (Globals::current_beat_pos % 8 == 0) // Globals::current_beat_pos holds 32 → %8 makes 4.
    {
      // Hardware::vibrate_motor(50);
    }
    // Debug: play MIDI note on quarter notes
    if (Globals::current_beat_pos % 8 == 0)
    {
      MIDI.sendNoteOn(57, 127, 2);
    }
    else
    {
      MIDI.sendNoteOff(57, 127, 2);
    }

    // --------------------------- 8th notes: -------------------------
    if (Globals::current_beat_pos % 4 == 0)
    {
      // increase 8th note counter:
      Globals::current_eighth_count = (Globals::current_eighth_count + 1) % 8;
      toggleLED = !toggleLED;

      // blink LED rhythmically:
      digitalWrite(LED_BUILTIN, toggleLED);
    }

    // --------------------------- 16th notes: ------------------------
    if (Globals::current_beat_pos % 2 == 0)
    {

      // increase 16th note counter:
      Globals::current_16th_count = (Globals::current_16th_count + 1) % 16;

      // vibrate if new score is ready:
      if (Effect::beat_sum.average_smooth > Effect::beat_sum.activation_thresh)
      {
        digitalWrite(VIBR, HIGH);
        Globals::println_to_console("ready to go to next score step! hit footswitch!");
      }
      else
        digitalWrite(VIBR, LOW);
    }

    // ----------------------------- draw play log to console

    // Globals::print_to_console(Globals::current_beat_pos / 4);
    // Globals::print_to_console("\t");
    // Globals::print_to_console(Globals::current_eighth_count);
    for (auto &instrument : instruments)
    {
      Globals::print_to_console(instrument->output_string);
      instrument->output_string = "\t";
    }
    Globals::println_to_console("");

    // print topo arrays:

    if (Globals::do_print_beat_sum)
    {
      Globals::printTopoArray(&Effect::beat_sum); // print volume layer
      // Globals::printTopoArray(&score1.beat_regularity);
    }

    // perform timed pin actions according to current beat:
    for (auto &instrument : instruments)
    {
      instrument->perform(instrument, instruments, MIDI);
    }
    ///////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////
    /////////////////////////// ABSTRACTIONS //////////////////////////
    ///////////////////////////////////////////////////////////////////

    // TODO:
    // Globals::topo_array_to_processing(&snare->topography);
    // Globals::topo_array_to_processing(&Effect::beat_sum);
    // Globals::topo_array_to_processing(&regularity);

    //makeTopo();
    // works like this:
    // instruments[0]->smoothen_dataArray(instruments[0]);
    // LOTS OF PSEUDOCODE HERE:

    //    beat_topography_16.smoothen(); // → gives you beat_topography.average_smoothened_height
    //
    //    if (beat_topography_16.average_smoothened_height >= beat_topography.threshold)
    //    {
    //      // create next abstraction layer
    //      int[] beat_regularity = new int[16];
    //
    //      // update abstraction layer:
    //      for (int i = 0; i < 16; i++)
    //        beat_regularity[i] = beat_topography_16[i];
    //
    //      // get average height again:
    //      beat_regularity.smoothen();
    //    }
    //
    //    if (beat_regularity.amplitude >= beat_regularity.threshold)
    //    {
    //      // you are playing super regularly. that's awesome. now activate next element in score
    //      score_next_element_ready = true;
    //    }
    //
    //    // if regularity is held for certain time (threshold reached):
    //    read_to_break
    //
    //    when THEN regularity expectations not fulfilled:
    //    score: go to next element
    //
    //    // elsewhere:
    //    if (hit_certain_trigger)
    //    {
    //
    //      if (score_next_element_ready)
    //      {
    //        /* do whatever is up next in the score */
    //      }
    //    }

    /* Score could look like this:
      intro----------part 1--...-----outro--------
      1     2     3     4    ...     20    21     step
      ++    ++    ++                              element_FX
                  ++    ++   ...     ++    ++     element_notes
                        ++           ++           element_fieldRecordings

      cool thing: create score dynamically according to how I play
    */

    // PSEUDOCODE END

    //////////////////////////////// SCORE ////////////////////////////
    ///////////////////////////////////////////////////////////////////

    // state proceeds if footswitch is pressed (in mode RESET_AND_PROCEED_SCORE) when regularity is high enough
    // static float delayDepth = 0;
    // switch (Globals::score_state)
    // {
    // case 1:
    static int noteLength = int(random(8) * 4);
    score1.continuousBassNote(MIDI, noteLength); // will play bass note from score repeatedly
    //   break;

    // case 2: // delay with Swell Effect on Snare
    //   snare->effect = Swell;
    //   snare->midi.cc_chan = DelayTime;
    //   delayDepth += 1; // fade in of delayDepth
    //   delayDepth = min(delayDepth, 127);
    //   MIDI.sendControlChange(DelayDepth, int(delayDepth), 2);
    //   // Globals::print_to_console("delayDepth = ");
    //   // Globals::println_to_console(delayDepth);
    //   // score1.crazyDelays(snare, MIDI);
    //   break;

    // case 3: // rawPin Delay Effect on Snare
    //   delayDepth -= 3;
    //   delayDepth = max(delayDepth, 0);

    //   MIDI.sendControlChange(DelayDepth, 50, 2);
    //   snare->effect = CC_Effect_rawPin;
    //   // score1.envelope_volume(&Effect::beat_sum, MIDI);
    //   break;

    // case 4: // note ascend
    //   standtom->topography.activation_thresh = 2;
    //   kick->topography.activation_thresh = 2;
    //   snare->topography.activation_thresh = 2;

    //   standtom->effect = Monitor;
    //   kick->effect = Monitor;
    //   snare->effect = Monitor;

    //   for (int i = 0; i < Globals::numInputs; i++)
    //   {
    //     if (instruments[i]->topography.average_smooth >= instruments[i]->topography.activation_thresh)
    //     {
    //       instruments[i]->midi.active_note += 3;
    //     }
    //   }
    //   break;

    // case 5: // envelope cutoff
    // snare->effect = TopographyLog;
    //   score1.envelope_cutoff(&Effect::beat_sum, MIDI);
    //   break;
    // }

    // continuousBassNote, quarterBassNotes, addBassNote,

    /////////////////////// AUXILIARY FUNCTIONS ///////////////////////

  } // end of (32nd-step) TIMED ACTIONS
  // ------------------------------------------------------------------

  ///////////////////////////// tidy up ///////////////////////////////
  last_beat_pos = Globals::current_beat_pos;
  Globals::last_eighth_count = Globals::current_eighth_count;
  Globals::last_16th_count = Globals::current_16th_count;

  // Hardware:
  Hardware::checkFootSwitch(instruments, &score1); // check state of footswitch
  // Hardware::request_motor_deactivation(); // turn off vibration and MIDI notes

  // tidying up what's left from performing functions..
  for (auto &instrument : instruments)
    instrument->tidyUp(instrument, MIDI);
}
// --------------------------------------------------------------------
