/*
   SUPER MUSCLE
   ------------------------------------
   October 2020-July 2021
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
#include <settings.h>
#include <Globals.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Score/Score.h>
#include <Serial.h>
#include <Rhythmics.h>
#include <Calibration.h>

// ----------------------------- settings -----------------------------
String VERSION_NUMBER = "0.2.105j";
const boolean DO_PRINT_JSON = false;
const boolean DO_PRINT_TO_CONSOLE = true;
const boolean DO_PRINT_BEAT_SUM = false;
const boolean DO_USE_RESPONSIVE_CALIBRATION = false;
const boolean USING_TSUNAMI = false;

// ----------------------------- variables ----------------------------
midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial &)Serial2); // same as MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);

// define instruments:

Synthesizer *mKorg; // create a KORG microKorg instrument called mKorg
Synthesizer *volca; // create a KORG Volca Keys instrument called volca

// Songs:
// Score *doubleSquirrel;
Score *monitoring;
Score *sattelstein;
Score *elektrosmoff;
Score *experimental;

Rhythmics *rhythmics;

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
      for (auto &instrument : Drumset::instruments)
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
  for (auto &instrument : Drumset::instruments)
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

  Globals::use_responsiveCalibration = DO_USE_RESPONSIVE_CALIBRATION;
  Globals::do_print_beat_sum = DO_PRINT_BEAT_SUM; // prints active_score->beat_sum topography array
  Globals::do_print_to_console = DO_PRINT_TO_CONSOLE;
  Globals::do_print_JSON = DO_PRINT_JSON;

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

  // LCD
  // Hardware::lcd->begin(16, 2);
  // Hardware::lcd->setCursor(0, 0);
  // Hardware::lcd->print("SUPER MUSCLE");
  // Hardware::lcd->setCursor(0, 1);
  // Hardware::lcd->print(VERSION_NUMBER);

  // delay(1000); // alternative to line above, if run with external power (no computer)

  // -------------------------------- MIDI ----------------------------
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // turn off all currently playing MIDI notes:
  for (int channel = 1; channel < 3; channel++)
  {
    for (int note_number = 0; note_number < 127; note_number++)
    {
      MIDI.sendNoteOff(note_number, 127, channel);
    }
  }

  // -------------------- Hardware initialization ---------------------

  if (USING_TSUNAMI)
  {
    delay(1000);              // wait for Tsunami to finish reset // redundant?
    Globals::tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
    delay(100);
    Globals::tsunami.stopAllTracks(); // in case Tsunami was already playing.
    Globals::tsunami.samplerateOffset(0, 0);
    Globals::tsunami.setReporting(true); // Enable track reporting from the Tsunami
    delay(100);                          // some time for Tsunami to respond with version string
  }

  // LCD
  Hardware::lcd->begin(16, 2);
  Hardware::lcd->setCursor(0, 0);
  Hardware::lcd->print("SUPER MUSCLE");
  Hardware::lcd->setCursor(0, 1);
  Hardware::lcd->print(VERSION_NUMBER);

  // ------------------------ INSTRUMENT SETUP ------------------------
  // instantiate external MIDI devices:
  mKorg = new Synthesizer(2);
  volca = new Synthesizer(1);

  Drumset::instruments = {Drumset::snare, Drumset::hihat, Drumset::kick, Drumset::tom2, Drumset::standtom, Drumset::crash1, Drumset::ride};

  rhythmics = new Rhythmics();

  // initialize arrays:
  for (auto &instrument : Drumset::instruments)
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

  // --------------- set instrument calibration array -----------------

  Drumset::snare->setup_sensitivity(SNARE_THRESHOLD, SNARE_CROSSINGS, SNARE_DELAY_AFTER_STROKE, SNARE_FIRST_STROKE);
  Drumset::hihat->setup_sensitivity(HIHAT_THRESHOLD, HIHAT_CROSSINGS, HIHAT_DELAY_AFTER_STROKE, HIHAT_FIRST_STROKE);
  Drumset::kick->setup_sensitivity(KICK_THRESHOLD, KICK_CROSSINGS, KICK_DELAY_AFTER_STROKE, KICK_FIRST_STROKE);
  // Drumset::tom1->setup_sensitivity(TOM1_THRESHOLD, TOM1_CROSSINGS, TOM1_DELAY_AFTER_STROKE, TOM1_FIRST_STROKE);
  Drumset::tom2->setup_sensitivity(TOM2_THRESHOLD, TOM2_CROSSINGS, TOM2_DELAY_AFTER_STROKE, TOM2_FIRST_STROKE);
  Drumset::standtom->setup_sensitivity(STANDTOM_THRESHOLD, STANDTOM_CROSSINGS, STANDTOM_DELAY_AFTER_STROKE, STANDTOM_FIRST_STROKE);
  Drumset::crash1->setup_sensitivity(CRASH1_THRESHOLD, CRASH1_CROSSINGS, CRASH1_DELAY_AFTER_STROKE, CRASH1_FIRST_STROKE);
  // Drumset::crash2->setup_sensitivity(CRASH2_THRESHOLD, CRASH2_CROSSINGS, CRASH2_DELAY_AFTER_STROKE, CRASH2_FIRST_STROKE);
  Drumset::ride->setup_sensitivity(RIDE_THRESHOLD, RIDE_CROSSINGS, RIDE_DELAY_AFTER_STROKE, RIDE_FIRST_STROKE);
  Drumset::cowbell->setup_sensitivity(COWBELL_THRESHOLD, COWBELL_CROSSINGS, COWBELL_DELAY_AFTER_STROKE, COWBELL_FIRST_STROKE);

  // ------------------ calculate noise floor -------------------------
  for (auto &instrument : Drumset::instruments)
    instrument->calculateNoiseFloor();

  // print startup information:
  Globals::println_to_console("\n..calculating noiseFloor done.");
  Globals::println_to_console("-----------------------------------------------");
  Globals::println_to_console("calibration values set as follows:");
  Globals::println_to_console("instr\tthrshld\tcrosses\tnoiseFloor");
  for (Instrument *instrument : Drumset::instruments)
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
  // doubleSquirrel = new Score("doubleSquirrel");
  monitoring = new Score("monitoring");
  sattelstein = new Score("sattelstein");
  sattelstein->setTempoRange(150, 170);
  elektrosmoff = new Score("elektrosmoff");
  experimental = new Score("experimental");
  Globals::score_list.push_back(sattelstein);
  Globals::score_list.push_back(elektrosmoff);
  Globals::score_list.push_back(experimental);
  Globals::active_score = sattelstein;

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
  Drumset::hihat->effect = Monitor;
  Drumset::crash1->effect = Monitor;
  Drumset::cowbell->effect = Monitor;
  Drumset::ride->effect = Monitor;

  // -------------------------- START TIMERS --------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond

  Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel

  if (Hardware::pushbutton_is_pressed())
    Globals::machine_state = Calibration;
  delay(2000);
  Hardware::lcd->clear();
  // delay(500);
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
  for (auto &instrument : Drumset::instruments)
  {
    // if (instrument->effect == PlayMidi_rawPin || instrument->effect == CC_Effect_rawPin)
    //  instrument->trigger(instrument, MIDI);

    if (instrument->stroke_detected()) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      instrument->trigger(MIDI);        // runs trigger function according to instrument's EffectType
      instrument->timing.wasHit = true; // a flag to show that the instrument was hit (for transmission via JSON)
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
  rhythmics->run_beat(last_beat_pos, Drumset::instruments, MIDI);

  //////////////////////////////// SCORE ////////////////////////////
  ///////////////////////////////////////////////////////////////////

  if (Globals::current_beat_pos != last_beat_pos) // run once per 32nd-step
  {
    // Globals::active_score->load();
    Globals::active_score->run(mKorg, MIDI); // TODO: globale Synthesizer-Liste

    //----------------------- SCORE END -------------------------------

    for (auto &instrument : Drumset::instruments)
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
  Hardware::checkFootSwitch(); // check step of footswitch
  // Hardware::request_motor_deactivation(); // turn off vibration and MIDI notes

  // rotary encoder:
  Hardware::checkEncoder();
  Hardware::checkPushButton();

  // LCD:
  Hardware::lcd_display();

  // tidying up what's left from performing functions..
  for (auto &instrument : Drumset::instruments)
    instrument->tidyUp(MIDI);

  // MIDI.sendControlChange(50, 100, 2);
}
// --------------------------------------------------------------------
