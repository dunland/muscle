#include <Instruments.h>
#include <MIDI.h>
// #include <Tsunami.h>
#include <Song.h>
#include <Hardware.h>
#include <settings.h>

// ---------------------- SETUP FUNCTIONS -----------------------------

// set instrument notes via list
void Instrument::set_notes(std::vector<int> list)
{
  for (uint8_t i = 0; i < list.size(); i++)
  {
    midi_settings.notes.push_back(list[i]);
  }
}

// setup midi settings with params
void Instrument::setup_midi(CC_Type cc_type, Synthesizer *synth, int cc_max, int cc_min, float cc_increase_factor, float cc_tidyUp_factor)
{
  midi_settings.cc_chan = cc_type;
  midi_settings.synth = synth;
  midi_settings.cc_max = cc_max;
  midi_settings.cc_min = cc_min;
  midi_settings.cc_increase_factor = cc_increase_factor;
  midi_settings.cc_tidyUp_factor = cc_tidyUp_factor;

  midi_settings.cc_standard = (cc_tidyUp_factor > 0) ? midi_settings.cc_min : midi_settings.cc_max; // standard value either cc_min or cc_max, depending on increasing or decreasing tidyUp-factor
}

// setup midi without params
// TODO: this one seems broken! fix it!
// void Instrument::setup_midi(CC_Type cc_type, Synthesizer *synth)
// {
//   midi_settings.cc_chan = cc_type;
//   midi_settings.synth = synth;
// }

// set instrument sensitivity
void Instrument::setup_sensitivity(int threshold_, int crossings_, int delayAfterStroke_, boolean countAfterFirstStroke)
{
  sensitivity.threshold = threshold_;
  sensitivity.crossings = crossings_;
  sensitivity.delayAfterStroke = delayAfterStroke_;
  timing.countAfterFirstStroke = countAfterFirstStroke;
}

// set effect without handle for variable:
void Instrument::set_effect(EffectsType effect_)
{
  Globals::print_to_console("Setting effect for ");
  Globals::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
  Globals::print_to_console(" to ");
  Globals::print_to_console(Globals::EffectstypeToHumanReadable(effect_));
  Globals::print_to_console("... ");

  switch (effect_)
  {
  case PlayMidi:
    if (midi_settings.notes.size() > 0 && midi_settings.active_note > 0)
    {
      effect = effect_;
      Globals::println_to_console("done.");
    }
    else
    {
      Globals::println_to_console("effect could not be set! no MIDI notes defined or no active_note defined for this instrument!");
    }
    break;

  case Reflex_and_PlayMidi:
    score.ready_to_shuffle = true;
    shuffle_cc(false);
    if (midi_settings.notes.size() > 0 && midi_settings.active_note > 0)
    {
      effect = effect_;
      Globals::println_to_console("done.");
    }
    else
    {
      Globals::println_to_console("effect could not be set! no MIDI notes defined or no active_note defined for this instrument!");
    }
    break;

  case Random_CC_Effect:
    score.ready_to_shuffle = true;
    shuffle_cc(false);
    effect = effect_;
    Globals::println_to_console("done.");
    break;

  case TapTempo:
    Globals::bSendMidiClock = true;
    effect = effect_;
    Globals::println_to_console("done.");
    break;

  default:
    effect = effect_;
    Globals::println_to_console("done.");
    break;
  }
}

///////////////////////// STROKE DETECTION /////////////////////////
////////////////////////////////////////////////////////////////////
bool Instrument::stroke_detected()
{
  // static unsigned long firstPinActiveTimeCopy[Globals::numInputs];
  // static int lastValue[Globals::numInputs];    // for LED toggle
  // static boolean toggleState = false; // for LED toggle

  noInterrupts();
  timing.lastPinActiveTimeCopy = timing.lastPinActiveTime;
  timing.firstPinActiveTimeCopy = timing.firstPinActiveTime;
  interrupts();

  boolean stroke_criterion = false;
  if (!timing.countAfterFirstStroke && millis() > timing.lastPinActiveTimeCopy + sensitivity.delayAfterStroke) // get counts only X ms after LAST hit
    stroke_criterion = true;

  if (timing.countAfterFirstStroke && millis() > timing.firstPinActiveTimeCopy + sensitivity.delayAfterStroke) // get counts only X ms after FIRST hit ??
    stroke_criterion = true;

  if (stroke_criterion)
  {
    static int countsCopy;
    noInterrupts();
    countsCopy = timing.counts;
    timing.counts = 0;
    interrupts();

    // ---------------------------- found significant count!
    if (countsCopy >= sensitivity.crossings)
    {
      // LED blink:
      // if (countsCopy[instr] != lastValue[instr]) toggleState = !toggleState;
      // digitalWrite(LED_BUILTIN, toggleState);
      // lastValue[instr] = countsCopy;

      // countsCopy[instr] = 0;

      return true;
    }
    else // timing ok but no significant counts
    {
      return false;
    }
  }
  else // TODO: timing not ok (obsolete: will always be ok!)
  {
    {
      return false;
    }
  }
}
// --------------------------------------------------------------------

////////////////////////////// CALCULATE NOISEFLOOR ///////////////////
///////////////////////////////////////////////////////////////////////
void Instrument::calculateNoiseFloor()
{
  // calculates the average noise floor out of 400 samples from all inputs
  long beginNoiseFloorCaluclation = millis();
  int led_idx = 0;

  Globals::print_to_console("calculating noiseFloor for ");
  Globals::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
  Globals::print_to_console(" (A");
  Globals::print_to_console(pin - 14);
  Globals::print_to_console(")");
  if (Globals::use_responsiveCalibration)
  {
    Globals::print_to_console(" ..waiting for stroke");
    Hardware::lcd->setCursor(0, 0);
    Hardware::lcd->print("waiting for stroke");
    Hardware::lcd->setCursor(0, 1);
    Hardware::lcd->print(Globals::DrumtypeToHumanreadable(drumtype));
    while (analogRead(pin) < min(1023, 700 + sensitivity.threshold))
      // timeout after 5s:
      if (millis() > beginNoiseFloorCaluclation + 5000)
      {
        int totalSamples = 0;
        boolean toggleState = false;
        for (int n = 0; n < 400; n++)
        {
          if (n % 100 == 0)
          {
            Globals::print_to_console(" . ");
            digitalWrite(led, toggleState);
            toggleState = !toggleState;
          }
          totalSamples += analogRead(pin);
        }
        break;
      };       // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
    Globals::print_to_console(" >!<");
    delay(1000); // should be long enough for drum not to oscillate anymore
  }

  int totalSamples = 0;
  boolean toggleState = false;
  for (int n = 0; n < 400; n++)
  {
    if (n % 100 == 0)
    {
      Globals::print_to_console(" . ");
      digitalWrite(led, toggleState);
      toggleState = !toggleState;
    }
    totalSamples += analogRead(pin);
  }
  sensitivity.noiseFloor = totalSamples / 400;
  digitalWrite(led, LOW);
  led_idx++;
  Globals::print_to_console("noiseFloor = ");
  Globals::println_to_console(sensitivity.noiseFloor);

  // turn LEDs off again:
  digitalWrite(led, LOW);
  output_string = "\t";
}
// --------------------------------------------------------------------

///////////////////// SET STRING FOR PLAY LOGGING /////////////////////
///////////////////////////////////////////////////////////////////////
void Instrument::setInstrumentPrintString()
{
  switch (effect)
  {
  case FootSwitchLooper: // add an ! if pinAction == 3 (replay logged rhythm)
    if (drumtype == Kick)
      output_string = "!■\t"; // Kickdrum
    else if (drumtype == Cowbell)
      output_string = "!▲\t"; // Cowbell
    else if (drumtype == Standtom1)
      output_string = "!□\t"; // Standtom1
    else if (drumtype == Standtom2)
      output_string = "!O\t"; // Standtom2
    else if (drumtype == Hihat)
      output_string = "!x\t"; // Hi-Hat
    else if (drumtype == Tom1)
      output_string = "!°\t"; // Tom 1
    else if (drumtype == Snare)
      output_string = "!※\t"; // Snaredrum
    else if (drumtype == Tom2)
      output_string = "!o\t"; // Tom 2
    else if (drumtype == Ride)
      output_string = "!xx\t"; // Ride
    else if (drumtype == Crash1)
      output_string = "!-X-\t"; // Crash1
    else if (drumtype == Crash2)
      output_string = "!-XX-\t"; // Crash2
    break;

  default: // just print what is being played
    if (drumtype == Kick)
      output_string = "■\t"; // Kickdrum
    else if (drumtype == Cowbell)
      output_string = "▲\t"; // Cowbell
    else if (drumtype == Standtom1)
      output_string = "□\t"; // Standtom1
    else if (drumtype == Standtom2)
      output_string = "O\t"; // Standtom2
    else if (drumtype == Hihat)
      output_string = "x\t"; // Hi-Hat
    else if (drumtype == Tom1)
      output_string = "°\t"; // Tom 1
    else if (drumtype == Snare)
      output_string = "※\t"; // Snaredrum
    else if (drumtype == Tom2)
      output_string = "o\t"; // Tom 2
    else if (drumtype == Ride)
      output_string = "xx\t"; // Ride
    else if (drumtype == Crash1)
      output_string = "-X-\t"; // Crash1
    else if (drumtype == Crash2)
      output_string = "-XX-\t"; // Crash2
    break;
  }
}

// --------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
////////////////////////// INSTRUMENT EVENTS //////////////////////////
///////////////////////////////////////////////////////////////////////

////////////////////////////// TRIGGERS ///////////////////////////////
void Instrument::trigger(midi::MidiInterface<HardwareSerial> MIDI)
{
  // print instrument name to receive using external programs via Serial connection:
  Serial.println(Globals::DrumtypeToHumanreadable(drumtype));
  Hardware::lcd->setCursor(12, 0);
  Hardware::lcd->print(Globals::DrumtypeToHumanreadable(drumtype));

  // TODO: use (list of) callback functions instead! (this way multiple things could be performed upon each hit)
  switch (effect)
  {
  case PlayMidi:
    monitor();
    playMidi(MIDI);
    break;

  case Monitor:
    monitor();
    break;

  case ToggleRhythmSlot:
    toggleRhythmSlot();
    break;

  case FootSwitchLooper:
    footswitch_recordSlots();
    break;

  case TapTempo:
    getTapTempo();
    break;

  case Swell:
    swell_rec(MIDI);
    break;

  // TODO:
  case TsunamiLink:
    // countup_topography(instrument);
    break;

  case CymbalSwell: // swell-effect for loudness on field recordings (use on cymbals e.g.)
    // TODO: UNTESTED! (2020-10-09)
    swell_rec(MIDI);
    break;
  case TopographyMidiEffect:
    // countup_topography(instrument);
    break;

  case Change_CC:
    change_cc_in(MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    monitor();
    break;

  case Random_CC_Effect:
    random_change_cc_in(MIDI);
    break;
  case MainNoteIteration:
    mainNoteIteration(midi_settings.synth, MIDI);
    break;

  case Reflex_and_PlayMidi: // combines PlayMidi and Change_CC
    playMidi(MIDI);
    change_cc_in(MIDI);
    break;

  default:
    break;
  }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

void Instrument::perform(std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial> MIDI)
{
  switch (effect)
  {

  case ToggleRhythmSlot:
    sendMidiNotes_timed(MIDI);
    break;

  case FootSwitchLooper:
    setInstrumentSlots();
    sendMidiNotes_timed(MIDI);
    break;

  case Swell:
    swell_perform(MIDI);
    break;

  case TopographyMidiEffect:
    topography_midi_effects(instruments, MIDI);
    break;

  default:
    break;
  }
}

/////////////////////////// TIDY UP FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////////////////
/* destructor for playing MIDI notes etc */

void Instrument::tidyUp(midi::MidiInterface<HardwareSerial> MIDI)
{
  // Globals::println_to_console("tidying up");
  switch (effect)
  {
  case PlayMidi:
    turnMidiNoteOff(MIDI);
    break;

  case Reflex_and_PlayMidi:
    turnMidiNoteOff(MIDI);
    shuffle_cc(false);
    break;

  case Change_CC:
    change_cc_out(MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    break;

  case Random_CC_Effect:
    change_cc_out(MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    shuffle_cc(false);
    break;

  default:
    break;
  }
}

// instantiate instruments:
Instrument *Drumset::snare = new Instrument(INPUT_PIN_SNARE, Snare);
Instrument *Drumset::hihat = new Instrument(INPUT_PIN_HIHAT, Hihat);
Instrument *Drumset::kick = new Instrument(INPUT_PIN_KICK, Kick);
Instrument *Drumset::tom1 = new Instrument(INPUT_PIN_TOM1, Tom1);
Instrument *Drumset::tom2 = new Instrument(INPUT_PIN_TOM2, Tom2);
Instrument *Drumset::standtom = new Instrument(INPUT_PIN_STANDTOM, Standtom1);
Instrument *Drumset::crash1 = new Instrument(INPUT_PIN_CRASH1, Crash1);
Instrument *Drumset::ride = new Instrument(INPUT_PIN_RIDE, Ride);

std::vector<Instrument *> Drumset::instruments = {Drumset::snare, Drumset::hihat, Drumset::kick, Drumset::tom2, Drumset::standtom, Drumset::tom1};

// instantiate external MIDI devices:
Synthesizer *Synthesizers::mKorg = new Synthesizer(MIDI_CHANNEL_MICROKORG, "mKRG");
Synthesizer *Synthesizers::volca = new Synthesizer(MIDI_CHANNEL_VOLCA, "vlca");
Synthesizer *Synthesizers::dd200 = new Synthesizer(MIDI_CHANNEL_DD200, "d200");
Synthesizer *Synthesizers::whammy = new Synthesizer(MIDI_CHANNEL_WHAMMY, "whmy");
Synthesizer *Synthesizers::kaossPad3 = new Synthesizer(MIDI_CHANNEL_KP3, "KP3");

std::vector<Synthesizer *> Synthesizers::synths = {Synthesizers::mKorg, Synthesizers::volca, Synthesizers::whammy, Synthesizers::dd200};