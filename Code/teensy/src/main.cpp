/*
   SUPER MUSCLE v.0.2.0
   ------------------------------------
   December 2020
   by David Unland david[at]unland[dot]eu
   github.com/dunland/muscle
   ------------------------------------
   ------------------------------------
*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
#include <Arduino.h>
#include <vector>
#include <MIDI.h>
#include <Tsunami.h>
#include <ArduinoJson.h>
#include <Globals.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Score.h>
#include <Serial.h>
#include <Rhythmics.h>

midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial &)Serial2); // same as MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// define instruments:

static std::vector<Instrument *> instruments; // all instruments go in here

Synthesizer *mKorg; // create a KORG microKorg instrument called mKorg
Synthesizer *volca; // create a KORG Volca Keys instrument called volca

Rhythmics *rhythmics;

Score *active_score;
Score *doubleSquirrel;
Score *elektrosmoff;

// ------------------------- interrupt timers -------------------------
IntervalTimer pinMonitor; // reads pins every 1 ms

/////////////////////////// general pin reading ///////////////////////
int pinValue(Instrument *instrument)
{
  return abs(instrument->sensitivity.noiseFloor - analogRead(instrument->pin));
}
// --------------------------------------------------------------------

//////////////////////////// PRINT NORMALIZED VALUES //////////////////
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

//////////////////// interrupt for sensor reading /////////////////////
void samplePins()
{
  // read all pins:
  for (auto &instrument : instruments)
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

///////////////////////////////////////////////////////////////////////////
////////////////////////////////// SETUP //////////////////////////////////
///////////////////////////////////////////////////////////////////////////

void setup()
{
  //---------------------- Global / Debug values ----------------------

  Globals::use_responsiveCalibration = false;
  Globals::do_print_beat_sum = false; // prints active_score->beat_sum topography array
  Globals::do_print_to_console = true;
  Globals::do_print_JSON = false;

  //------------------------ initialize pins --------------------------
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);
  pinMode(PUSHBUTTON, INPUT_PULLUP);

  randomSeed(analogRead(A0) * analogRead(A19));

  //-------------------------- Communication --------------------------

  Serial.begin(115200);
  // Serial3.begin(57600); // contained in tsunami.begin()
  unsigned long wait_for_Serial = millis();

  while (!Serial) // prevents Serial flow from just stopping at some (early) point.
  {
    if (millis() > wait_for_Serial + 5000)
    {
      Globals::do_print_JSON = false;
      Globals::do_print_to_console = false;
      break;
    }
  }
  // delay(1000); // alternative to line above, if run with external power (no computer)

  MIDI.begin(MIDI_CHANNEL_OMNI);

  // -------------------- Hardware initialization ---------------------
  // Hardware::lcd(Hardware::RS, Hardware::EN, Hardware::D4, Hardware::D5, Hardware::D6, Hardware::D7);

  delay(1000);              // wait for Tsunami to finish reset // redundant?
  Globals::tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
  delay(100);
  Globals::tsunami.stopAllTracks(); // in case Tsunami was already playing.
  Globals::tsunami.samplerateOffset(0, 0);
  Globals::tsunami.setReporting(true); // Enable track reporting from the Tsunami
  delay(100);                          // some time for Tsunami to respond with version string

  // LCD
  Hardware::lcd->begin(16, 2);

  // ------------------------ INSTRUMENT SETUP ------------------------
  // instantiate external MIDI devices:
  mKorg = new Synthesizer(2);
  volca = new Synthesizer(1);


  instruments = {Drumset::snare, Drumset::hihat, Drumset::kick, Drumset::tom2, Drumset::standtom, Drumset::crash1, Drumset::ride};

  rhythmics = new Rhythmics();

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

  // set instrument calibration array:

  // tom1->sensitivity.threshold = 200;
  // tom1->sensitivity.crossings = 20;
  Drumset::hihat->setup_sensitivity(80, 15, 10, false);
  Drumset::standtom->setup_sensitivity(200, 10, 10, false); // (2020-11-11) // 60, 20 (2020-08-27)
  Drumset::tom2->setup_sensitivity(100, 9, 10, false);      // 300, 18
  Drumset::kick->setup_sensitivity(200, 12, 10, false);     // (2020-11-11) // 100, 16 (2020-08-27)
  Drumset::cowbell->setup_sensitivity(80, 15, 10, false);
  Drumset::crash1->setup_sensitivity(300, 2, 5, true);
  Drumset::ride->setup_sensitivity(400, 2, 5, true);
  Drumset::snare->setup_sensitivity(120, 10, 10, false); // (2020-11-11) // 180, 12 (2020-08-27)

  // calculate noise floor:
  for (auto &instrument : instruments)
    instrument->calculateNoiseFloor();

  // print startup information:
  Globals::println_to_console("\n..calculating noiseFloor done.");

  Globals::println_to_console("assigning effects...");

  Globals::println_to_console("-----------------------------------------------");
  Globals::println_to_console("calibration values set as follows:");
  Globals::println_to_console("instr\tthrshld\tcrosses\tnoiseFloor");
  for (Instrument *instrument : instruments)
  {
    Globals::print_to_console(Globals::DrumtypeToHumanreadable(instrument->drumtype));
    Globals::print_to_console("\t");
    Globals::print_to_console(instrument->sensitivity.threshold);
    Globals::print_to_console("\t");
    Globals::print_to_console(instrument->sensitivity.crossings);
    Globals::print_to_console("\t");
    Globals::println_to_console(instrument->sensitivity.noiseFloor);
  }
  Globals::println_to_console("-----------------------------------------------");

  // ---------------------------------- SCORE -------------------------
  doubleSquirrel = new Score();
  elektrosmoff = new Score();
  active_score = doubleSquirrel;

  // turn off all currently playing MIDI notes:
  for (int channel = 1; channel < 3; channel++)
  {
    for (int note_number = 0; note_number < 127; note_number++)
    {
      MIDI.sendNoteOff(note_number, 127, channel);
    }
  }

  // link midi synth to instruments:
  Drumset::snare->midi_settings.synth = mKorg;
  Drumset::kick->midi_settings.synth = mKorg;
  Drumset::hihat->midi_settings.synth = mKorg;
  Drumset::crash1->midi_settings.synth = mKorg;
  Drumset::ride->midi_settings.synth = mKorg;
  Drumset::tom2->midi_settings.synth = mKorg;
  Drumset::standtom->midi_settings.synth = mKorg;
  Drumset::cowbell->midi_settings.synth = mKorg;

  // an initial midi note must be defined, otherwise there is a problem with the tidyUp function
  // Drumset::snare->midi_settings.active_note = 50;
  // Drumset::kick->midi_settings.active_note = 50;
  // Drumset::hihat->midi_settings.active_note = 50;
  // Drumset::crash1->midi_settings.active_note = 50;
  // Drumset::ride->midi_settings.active_note = 50;
  // Drumset::tom2->midi_settings.active_note = 50;
  // Drumset::standtom->midi_settings.active_note = 50;
  // Drumset::cowbell->midi_settings.active_note = 50;

  // assign startup instrument effects:
  Drumset::hihat->effect = TapTempo;
  Drumset::crash1->effect = Monitor;
  Drumset::cowbell->effect = Monitor;
  Drumset::ride->effect = Monitor;

  // -------------------------- START TIMERS --------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond

  Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel
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
      instrument->trigger(MIDI, active_score); // runs trigger function according to instrument's EffectType
      instrument->timing.wasHit = true;        // a flag to show that the instrument was hit (for transmission via JSON)
    }
  }

  ///////////////////////////// TIMED ACTIONS /////////////////////////
  // (automatically invoke rhythm-linked actions)
  static int last_beat_pos = 0;
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
  rhythmics->run_beat(last_beat_pos, instruments, MIDI, active_score);

  //////////////////////////////// SCORE ////////////////////////////
  ///////////////////////////////////////////////////////////////////

  // step proceeds if footswitch is pressed (in mode RESET_AND_PROCEED_SCORE) when regularity is high enough

  // THIS SONG IS COMPOSED FOR microKORG A.63
  // SCORE, stepwise:

  static std::vector<int> locrian_mode = {active_score->notes[0] + 1, active_score->notes[0] + 3, active_score->notes[0] + 5, active_score->notes[0] + 6, active_score->notes[0] + 8, active_score->notes[0] + 11};

  if (Globals::current_beat_pos != last_beat_pos)
  {
    // active_score->run_doubleSquirrel(active_score, MIDI, mKorg, volca, Drumset::hihat, Drumset::snare, Drumset::kick, Drumset::tom2, Drumset::ride, Drumset::crash1, Drumset::standtom); // TODO: manual assignment of functions, automatic access to instruments etc!!!

    // active_score->run();
    active_score->run_experimental(mKorg, volca);

    // vibrate if new score is ready:
    if (active_score->beat_sum.ready())
    {
      digitalWrite(VIBR, HIGH);
      Globals::println_to_console("ready to go to next score step! hit footswitch!");
    }
    else
      digitalWrite(VIBR, LOW);

    //----------------------- SCORE END -------------------------------

    for (auto &instrument : instruments)
    {
      instrument->timing.wasHit = false;
    }

  } // end of (32nd-step) TIMED ACTIONS
  // ------------------------------------------------------------------

  ///////////////////////////// tidy up ///////////////////////////////
  last_beat_pos = Globals::current_beat_pos;
  Globals::last_eighth_count = Globals::current_eighth_count;
  Globals::last_16th_count = Globals::current_16th_count;

  // Hardware:
  Hardware::checkFootSwitch(instruments, active_score); // check step of footswitch
  // Hardware::request_motor_deactivation(); // turn off vibration and MIDI notes
  // rotary encoder:
  Hardware::checkEncoder();

  // tidying up what's left from performing functions..
  for (auto &instrument : instruments)
    instrument->tidyUp(MIDI);

  // MIDI.sendControlChange(50, 100, 2);
}
// --------------------------------------------------------------------
