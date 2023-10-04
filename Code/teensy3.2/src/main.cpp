/*
   SUPER MUSCLE
   ------------------------------------
   October 2020-September 2023
   by David Unland email[at]davidunland[dot]de
   github.com/dunland/muscle
   ------------------------------------
   ------------------------------------
*/

/* --------------------------------------------------------------------
 --------------------------------- GLOBAL -----------------------------
 ------------------------------------------------------------------- */

#include <Arduino.h>
#include <vector>
// #include <Tsunami.h>
#include <ArduinoJson.h>
#include <settings.h>
#include <Globals.h>
#include <Devtools.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Song.h>
#include <Serial.h>
#include <JSON.h>
#include <Rhythmics.h>
#include <Calibration.h>
#include <SD.h>

const String VERSION_NUMBER = "1.0.0";

//---------------------- Global / Debug values ----------------------

const boolean USING_TSUNAMI = false;
boolean Devtools::use_responsiveCalibration = false;
boolean Devtools::do_print_beat_sum = false; // prints active_score->beat_sum topography array
boolean Devtools::do_print_to_console = true;
boolean Devtools::do_print_JSON = false;
boolean Devtools::do_send_to_processing = false;
boolean Devtools::printStrokes = true;

// ----------------------------- variables ----------------------------

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
        Devtools::print_to_console(pinValue(instrument));
        Devtools::print_to_console("\t");
      }
      Devtools::println_to_console("");
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
  //------------------------ initialize pins --------------------------
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);
  pinMode(PUSHBUTTON, INPUT_PULLUP);
  // HINT: all analog pins are inputs anyway; they don't have to be set up

  randomSeed(analogRead(A0) * analogRead(19));

  //-------------------------- Communication --------------------------

  Serial.begin(115200);
  // Serial3.begin(57600); // contained in tsunami.begin()
  unsigned long wait_for_Serial = millis();

  while (!Serial) // prevents Serial flow from just stopping at some (early) point.
  {

    if (millis() > wait_for_Serial + STARTUP_TIME_WAIT_FOR_USB)
    {
      Devtools::do_print_JSON = false;
      Devtools::do_print_to_console = false;
      break;
    }
  }

  SD.begin(BUILTIN_SDCARD);

  // -------------------------------- MIDI ----------------------------
  Hardware::begin_MIDI();

  // -------------------- Hardware initialization ---------------------

  // if (USING_TSUNAMI)
  // {
  //   delay(1000);              // wait for Tsunami to finish reset // redundant?
  //   Globals::tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
  //   delay(100);
  //   Globals::tsunami.stopAllTracks(); // in case Tsunami was already playing.
  //   Globals::tsunami.samplerateOffset(0, 0);
  //   Globals::tsunami.setReporting(true); // Enable track reporting from the Tsunami
  //   delay(100);                          // some time for Tsunami to respond with version string
  // }

  // LCD
  Hardware::lcd->begin(16, 2);
  Hardware::lcd->setCursor(0, 0);
  Hardware::lcd->print("SUPER MUSCLE");
  Hardware::lcd->setCursor(0, 1);
  Hardware::lcd->print(VERSION_NUMBER);

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
  Drumset::tom1->setup_sensitivity(TOM1_THRESHOLD, TOM1_CROSSINGS, TOM1_DELAY_AFTER_STROKE, TOM1_FIRST_STROKE);

  if (!JSON::read_sensitivity_data_from_SD(Drumset::instruments))
  {
    Globals::bUsingSDCard = true;
  }

  // ------------------ calculate noise floor -------------------------
  for (auto &instrument : Drumset::instruments)
    instrument->calculateNoiseFloor();

  // print startup information:
  Devtools::println_to_console("\n..calculating noiseFloor done.");
  Devtools::println_to_console("-----------------------------------------------");
  Devtools::println_to_console("calibration values set as follows:");
  Devtools::println_to_console("instr\tthrshld\tcrosses\tnoiseFloor");
  for (Instrument *instrument : Drumset::instruments)
  {
    Devtools::print_to_console(Globals::DrumtypeToHumanreadable(instrument->drumtype));
    Devtools::print_to_console("\t");
    Devtools::print_to_console(instrument->sensitivity.threshold);
    Devtools::print_to_console("\t");
    Devtools::print_to_console(instrument->sensitivity.crossings);
    Devtools::print_to_console("\t");
    Devtools::println_to_console(instrument->sensitivity.noiseFloor);
  }
  Devtools::println_to_console("-----------------------------------------------");

  // -------------------------------- songlist ------------------------
  //   Globals::songlist.push_back(new Song(run_b_11, "b_11"));
  // Globals::songlist.push_back(new Song(run_b_73, "b_73"));
  // Globals::songlist.push_back(new Song(run_b_63, "b_63"));
  // Globals::songlist.push_back(new Song(run_A_15, "A_15"));
  // Globals::songlist.push_back(new Song(run_A_25, "A_25"));
  // Globals::songlist.push_back(new Song(run_A_72, "A_72"));
  // Globals::songlist.push_back(new Song(run_b_27, "b_27"));
  // Globals::songlist.push_back(new Song(run_b_36, "b_36"));

  Globals::songlist.push_back(new Song(run_monitoring, "monitoring"));
  Globals::songlist.push_back(new Song(run_A_72, "intro"));
  Globals::songlist.push_back(new Song(run_PogoNumberOne, "pogoNumberOne"));
  Globals::songlist.push_back(new Song(run_hutschnur, "hutschnur"));
  Globals::songlist.push_back(new Song(run_randomSelect, "randomSelect"));
  Globals::songlist.push_back(new Song(run_wueste, "wueste"));
  Globals::songlist.push_back(new Song(run_randomSelect, "randomSelect"));
  Globals::songlist.push_back(new Song(run_besen, "besen"));
  Globals::songlist.push_back(new Song(run_randomSelect, "randomSelect"));
  Globals::songlist.push_back(new Song(run_alhambra, "alhambra"));
  Globals::songlist.push_back(new Song(run_randomSelect, "randomSelect"));
  Globals::songlist.push_back(new Song(run_monitoring, "mrWimbledon")); // mrWimbledon
  Globals::songlist.push_back(new Song(run_roeskur, "roeskur"));
  Globals::songlist.push_back(new Song(run_sattelstein, "sattelstein"));
  Globals::songlist.push_back(new Song(run_theodolit, "theodolit"));
  Globals::songlist.push_back(new Song(run_kupferUndGold, "kupferUndGold"));
  Globals::songlist.push_back(new Song(run_donnerwetter, "donnerwetter"));
  Globals::songlist.push_back(new Song(run_randomVoice, "randomSelect"));

  // Globals::songlist.at(sizeof(Globals::songlist))->setTempoRange(150, 170); // TODO: make this work!
  // Globals::songlist.push_back(new Song(run_host));
  // Globals::songlist.push_back(new Song(run_randomVoice));
  // Globals::songlist.push_back(new Song(run_control_dd200));
  // Globals::songlist.push_back(new Song(run_dd200_timeControl));
  // Globals::songlist.push_back(new Song(run_whammyMountains));
  // Globals::songlist.push_back(new Song(run_control_volca));
  // Globals::songlist.push_back(new Song(run_visuals));
  // Globals::songlist.push_back(new Song(run_nanokontrol));

  Globals::active_song = Globals::songlist.at(0);

  // link midi synth to instruments:
  Drumset::snare->midi.synth = Synthesizers::mKorg;
  Drumset::kick->midi.synth = Synthesizers::mKorg;
  Drumset::hihat->midi.synth = Synthesizers::mKorg;
  Drumset::crash1->midi.synth = Synthesizers::mKorg;
  Drumset::ride->midi.synth = Synthesizers::mKorg;
  Drumset::tom1->midi.synth = Synthesizers::mKorg;
  Drumset::tom2->midi.synth = Synthesizers::mKorg;
  Drumset::standtom->midi.synth = Synthesizers::mKorg;

  // an initial midi note must be defined, otherwise there is a problem with the tidyUp function
  // Drumset::snare->midi.active_note = 50;
  // Drumset::kick->midi.active_note = 50;
  // Drumset::hihat->midi.active_note = 50;
  // Drumset::crash1->midi.active_note = 50;
  // Drumset::ride->midi.active_note = 50;
  // Drumset::tom2->midi.active_note = 50;
  // Drumset::standtom->midi.active_note = 50;
  // Drumset::cowbell->midi.active_note = 50;

  // assign startup instrument effects:
  Drumset::hihat->effect = Monitor;
  Drumset::crash1->effect = Monitor;
  Drumset::tom1->effect = Monitor;
  Drumset::ride->effect = Monitor;

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel

  if (Hardware::pushbutton_state()) // pushbutton is pressed
  {
    noInterrupts();
    Globals::machine_state = Calibrating;
    interrupts();
    Hardware::lcd->clear();
    Hardware::lcd->setCursor(0, 0);
    Hardware::lcd->print("entering");
    Hardware::lcd->setCursor(0, 1);
    Hardware::lcd->print("Calibration Mode");
    Calibration::setup();
    Serial.println("Entering Calibration Mode.");
    delay(1000);
  }
  else
  {
    noInterrupts();
    Globals::machine_state = Running;
    interrupts();
  }

  // -------------------------- START TIMERS --------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond

  Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // wait another second
  delay(1000);
  Hardware::lcd->clear();
  // delay(500);
}

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{
  // Globals::tsunami.update(); // keeps variables for playing tracks etc up to date

  // ------------------------- DEBUG AREA -----------------------------
  printNormalizedValues(false);

  // --------------------- INCOMING SIGNALS FROM PIEZOS ---------------
  // (define what should happen when instruments are hit)
  for (auto &instrument : Drumset::instruments)
  {
    // if (instrument->effect == PlayMidi_rawPin || instrument->effect == CC_Effect_rawPin)
    //  instrument->trigger(instrument;

    if (instrument->stroke_detected()) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      instrument->trigger();            // runs trigger function according to instrument's EffectType
      instrument->timing.wasHit = true; // a flag to show that the instrument was hit (for transmission via JSON)

      Synthesizers::volca->sendNoteOn(instrument->midi.notes[0]);

      instrument->timing.lastHit = millis();
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
  if (sendMidiCopy) // MIDI-clock has to be sent 24 times per quarter note
  {
    if (Globals::bSendMidiClock)
      Hardware::sendMidiClock();
    noInterrupts();
    Globals::sendMidiClock = false;
    interrupts();
  }

  //----------------------- DO THINGS ONCE PER 32nd-step:--------------
  rhythmics->run_beat(last_beat_pos, Drumset::instruments);

  //////////////////////////////// RHYTHMICS //////////////////////////
  /////////////////////////////////////////////////////////////////////

  if (Globals::current_beat_pos != last_beat_pos) // run once per 32nd-step
  {
    Globals::active_song->trigger_function();

    //----------------------- RHYTHMICS END ---------------------------

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

  // rotary encoder:
  Hardware::checkEncoder();
  Hardware::checkPushButton();

  // LCD:
  Hardware::lcd_display();

  if (Globals::machine_state == Calibrating)
  {
    Calibration::update();
  }

  // tidying up what's left from performing functions..
  for (auto &instrument : Drumset::instruments)
  {
    // tidying up what's left from performing functions..
    instrument->tidyUp();

    // tentative: turn volca note off after instruments' specific stroke threshold
    if (millis() > instrument->timing.lastHit + instrument->sensitivity.delayAfterStroke * 30)
      Synthesizers::volca->sendNoteOff(instrument->midi.notes[0]);
  }

  if (Devtools::do_print_to_console)
    NanoKontrol::loop();
}
// --------------------------------------------------------------------