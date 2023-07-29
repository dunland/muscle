/*
   SUPER MUSCLE
   ------------------------------------
   October 2020-April 2023
   by David Unland david[at]davidunland[dot]de
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
// #include <Tsunami.h>
#include <settings.h>
#include <Globals.h>
#include <Devtools.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Song.h>
#include <Serial.h>
#include <Rhythmics.h>
#include <Calibration.h>

// ----------------------------- settings -----------------------------
const String VERSION_NUMBER = "0.2.39";
const boolean DO_PRINT_JSON = false;
const boolean DO_PRINT_TO_CONSOLE = true;
const boolean DO_PRINT_BEAT_SUM = false;
const boolean DO_USE_RESPONSIVE_CALIBRATION = false;
const boolean USING_TSUNAMI = false;

// ----------------------------- variables ----------------------------
midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial &)Serial2); // same as MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI);
// typedef midi::MidiInterface<HardwareSerial> MidiInterface;

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
  //---------------------- Global / Debug values ----------------------

  Devtools::use_responsiveCalibration = DO_USE_RESPONSIVE_CALIBRATION;
  Devtools::do_print_beat_sum = DO_PRINT_BEAT_SUM; // prints active_score->beat_sum topography array
  Devtools::do_print_to_console = DO_PRINT_TO_CONSOLE;
  Devtools::do_print_JSON = DO_PRINT_JSON;

  //------------------------ initialize pins --------------------------
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);
  pinMode(PUSHBUTTON, INPUT_PULLUP);

  randomSeed(analogRead(A0) * analogRead(A1));

  //-------------------------- Communication --------------------------

  Serial.begin(9600);
  // Serial3.begin(57600); // contained in tsunami.begin()
  unsigned long wait_for_Serial = millis();

  while (!Serial) // prevents Serial flow from just stopping at some (early) point.
  {

    if (millis() > wait_for_Serial + 5000)
    {
      Devtools::do_print_JSON = false;
      Devtools::do_print_to_console = false;
      break;
    }
  }

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
  //   Globals::songlist.push_back(new Song(std::bind(run_b_11, MIDI), "b_11"));
  // Globals::songlist.push_back(new Song(std::bind(run_b_73, MIDI), "b_73"));
  // Globals::songlist.push_back(new Song(std::bind(run_b_63, MIDI), "b_63"));
  // Globals::songlist.push_back(new Song(std::bind(run_A_15, MIDI), "A_15"));
  // Globals::songlist.push_back(new Song(std::bind(run_A_25, MIDI), "A_25"));
  // Globals::songlist.push_back(new Song(std::bind(run_A_72, MIDI), "A_72"));
  // Globals::songlist.push_back(new Song(std::bind(run_b_27, MIDI), "b_27"));
  // Globals::songlist.push_back(new Song(std::bind(run_b_36, MIDI), "b_36"));

  Globals::songlist.push_back(new Song(std::bind(run_A_72, MIDI), "A_72"));
  Globals::songlist.push_back(new Song(std::bind(run_roeskur, MIDI), "roeskur"));
  Globals::songlist.push_back(new Song(std::bind(run_randomSelect, MIDI), "randomSelect"));
  Globals::songlist.push_back(new Song(std::bind(run_theodolit, MIDI), "theodolit"));
  Globals::songlist.push_back(new Song(std::bind(run_randomSelect, MIDI), "randomSelect"));
  Globals::songlist.push_back(new Song(std::bind(run_monitoring, MIDI), "mrWimbledon"));
  Globals::songlist.push_back(new Song(std::bind(run_sattelstein, MIDI), "sattelstein"));
  Globals::songlist.push_back(new Song(std::bind(run_kupferUndGold, MIDI), "kupferUndGold"));
  Globals::songlist.push_back(new Song(std::bind(run_donnerwetter, MIDI), "donnerwetter"));
  Globals::songlist.push_back(new Song(std::bind(run_randomVoice, MIDI), "randomVoice"));
  // Globals::songlist.push_back(new Song(std::bind(run_randomSelect, MIDI), "randomSelect"));
  // Globals::songlist.push_back(new Song(std::bind(run_intro, MIDI), "intro"));
  // Globals::songlist.push_back(new Song(std::bind(run_PogoNumberOne, MIDI), "pogoNumberOne"));
  // Globals::songlist.push_back(new Song(std::bind(run_hutschnur, MIDI), "hutschnur"));
  // Globals::songlist.push_back(new Song(std::bind(run_randomSelect, MIDI), "randomSelect"));
  // Globals::songlist.push_back(new Song(std::bind(run_wueste, MIDI), "wueste"));
  // Globals::songlist.push_back(new Song(std::bind(run_besen, MIDI), "besen"));
  // Globals::songlist.push_back(new Song(std::bind(run_alhambra, MIDI), "alhambra"));

  // Globals::songlist.at(sizeof(Globals::songlist))->setTempoRange(150, 170); // TODO: make this work!
  // Globals::songlist.push_back(new Song(std::bind(run_host, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_randomVoice, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_control_dd200, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_dd200_timeControl, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_whammyMountains, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_control_volca, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_visuals, MIDI)));
  // Globals::songlist.push_back(new Song(std::bind(run_nanokontrol, MIDI)));

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

  // -------------------------- START TIMERS --------------------------
  pinMonitor.begin(samplePins, 1000); // sample pin every 1 millisecond

  Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // Debug:
  // tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel

  // if (Hardware::pushbutton_is_pressed())
  // Globals::machine_state = NanoKontrol_Test; // Debug Nanokontrol LCD
  delay(2000);
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
    //  instrument->trigger(instrument, MIDI);

    if (instrument->stroke_detected()) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      instrument->trigger(MIDI);        // runs trigger function according to instrument's EffectType
      instrument->timing.wasHit = true; // a flag to show that the instrument was hit (for transmission via JSON)

      Synthesizers::volca->sendNoteOn(instrument->midi.notes[0], MIDI);

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
    // Serial2.write(0xF8);
    // Serial2.write(midi::Clock);
    if (Globals::bSendMidiClock)
      MIDI.sendRealTime(midi::Clock);
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
    Globals::active_song->trigger_function(MIDI);

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

  // rotary encoder:
  Hardware::checkEncoder();
  Hardware::checkPushButton();

  // LCD:
  Hardware::lcd_display();

  for (auto &instrument : Drumset::instruments)
  {
    // tidying up what's left from performing functions..
    instrument->tidyUp(MIDI);

    // tentative: turn volca note off after instruments' specific stroke threshold
    if (millis() > instrument->timing.lastHit + instrument->sensitivity.delayAfterStroke * 30)
      Synthesizers::volca->sendNoteOff(instrument->midi.notes[0], MIDI);
  }

  NanoKontrol::loop();
}
// --------------------------------------------------------------------
