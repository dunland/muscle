#include <Instruments.h>
#include <MIDI.h>
#include <Tsunami.h>
#include <Score.h>
#include <Hardware.h>

void Instrument::set_notes(std::vector<int> list)
{
  for (uint8_t i = 0; i < list.size(); i++)
  {
    midi_settings.notes.push_back(list[i]);
  }
}

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

void Instrument::setup_midi(CC_Type cc_type, Synthesizer *synth) // setup midi without params
{
  midi_settings.cc_chan = cc_type;
  midi_settings.synth = synth;
}

void Instrument::setup_sensitivity(int threshold_, int crossings_, int delayAfterStroke_, boolean countAfterFirstStroke)
{
  sensitivity.threshold = threshold_;
  sensitivity.crossings = crossings_;
  sensitivity.delayAfterStroke = delayAfterStroke_;
  timing.countAfterFirstStroke = countAfterFirstStroke;
}

// without handle for variable:
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

  case Random_CC_Effect:
    score.ready_to_shuffle = true;
    shuffle_cc();
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

  if (timing.countAfterFirstStroke && millis() > timing.firstPinActiveTimeCopy + sensitivity.delayAfterStroke) //get counts only X ms after FIRST hit ??
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
      //if (countsCopy[instr] != lastValue[instr]) toggleState = !toggleState;
      //digitalWrite(LED_BUILTIN, toggleState);
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

  int led_idx = 0;

  Globals::print_to_console("calculating noiseFloor for ");
  Globals::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
  Globals::print_to_console(" (A");
  Globals::print_to_console(pin - 14);
  Globals::print_to_console(")");
  if (Globals::use_responsiveCalibration)
  {
    Globals::print_to_console(" ..waiting for stroke");
    while (analogRead(pin) < min(1023, 700 + sensitivity.threshold))
      ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
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
void Instrument::trigger(midi::MidiInterface<HardwareSerial> MIDI, Score *active_score)
{
  // always count up topography:
  countup_topography();
  topography.smoothen_dataArray();

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

  //TODO:
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
    break;

  case Random_CC_Effect:
    random_change_cc_in(MIDI);
    break;
  case MainNoteIteration:
    mainNoteIteration(midi_settings.synth, MIDI, active_score);
    break;

  default:
    break;
  }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

void Instrument::perform(std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial> MIDI, Score *active_score)
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
    topography_midi_effects(instruments, MIDI, active_score);
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

  case Change_CC:
    change_cc_out(MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    break;

  case Random_CC_Effect:
    change_cc_out(MIDI); // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
    shuffle_cc();
    break;

  default:
    break;
  }
}

// TODO: split these to different header files

///////////////////////////////////////////////////////////////////////
////////////////////////////// EFFECTS ////////////////////////////////
///////////////////////////////////////////////////////////////////////

///////////////////////////// TRIGGER EFFECTS /////////////////////////

void Instrument::change_cc_in(midi::MidiInterface<HardwareSerial> MIDI) // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
{
  midi_settings.cc_val += midi_settings.cc_increase_factor;
  midi_settings.cc_val = min(midi_settings.cc_val, midi_settings.cc_max);
  midi_settings.synth->sendControlChange(midi_settings.cc_chan, int(min(midi_settings.cc_val, 127)), MIDI);
  output_string = String(midi_settings.cc_val);
  output_string += "\t";
}

void Instrument::random_change_cc_in(midi::MidiInterface<HardwareSerial> MIDI) // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
{
  midi_settings.cc_val += midi_settings.cc_increase_factor;
  midi_settings.cc_val = min(midi_settings.cc_val, midi_settings.cc_max);
  midi_settings.synth->sendControlChange(midi_settings.random_cc_chan, int(min(midi_settings.cc_val, 127)), MIDI);
  output_string = String(midi_settings.cc_val);
  output_string += "\t";
}

// plays a Midi note:
void Instrument::playMidi(midi::MidiInterface<HardwareSerial> MIDI)
{
  midi_settings.synth->sendNoteOn(midi_settings.active_note, MIDI);
  score.last_notePlayed = millis();
}

void Instrument::monitor() // just prints what is being played.
{
  if (Globals::printStrokes)
  {
    setInstrumentPrintString();
  }
}

void Instrument::toggleRhythmSlot()
{
  if (Globals::printStrokes)
    setInstrumentPrintString();
  score.read_rhythm_slot[Globals::current_eighth_count] = !score.read_rhythm_slot[Globals::current_eighth_count];
}

void Instrument::footswitch_recordSlots() // record what is being played and replay it later
{
  if (Globals::printStrokes)
    setInstrumentPrintString();
  score.set_rhythm_slot[Globals::current_eighth_count] = true;
}

void Instrument::getTapTempo()
{
  static unsigned long timeSinceFirstTap = 0;
  static int tapState = 1;
  static int num_of_taps = 0;
  static int clock_sum = 0;

  switch (tapState)
  {
    //    case 0: // this is for activation of tap tempo listen
    //      tapState = 1;
    //      break;

  case 1:                                     // first hit
    if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
    {
      num_of_taps = 0;
      clock_sum = 0;
      Globals::println_to_console("-----------TAP RESET!-----------\n");
    }
    timeSinceFirstTap = millis(); // record time of first hit
    tapState = 2;                 // next: wait for second hit

    break;

  case 2: // second hit

    if (millis() < timeSinceFirstTap + 2000) // only record tap if interval was not too long
    {
      num_of_taps++;
      clock_sum += millis() - timeSinceFirstTap;
      Globals::tapInterval = clock_sum / num_of_taps;
      Globals::print_to_console("new tap Tempo is ");

      if (Globals::tapInterval >= 300) // quarter notes when slow tapping
      {
        Globals::current_BPM = 60000 / Globals::tapInterval;
        Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        Globals::print_to_console(60000 / Globals::tapInterval);
        Globals::print_to_console(" bpm (");
        Globals::print_to_console(Globals::tapInterval);
        Globals::println_to_console(" ms interval int quarter-notes)");
      }
      else // eighth-notes when fast tapping
      {
        Globals::current_BPM = (60000 / Globals::tapInterval) / 2;                                    // BPM >= 180 → strokes are 8th-notes, BPM half-time
        Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 8 / 128); // 8 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        Globals::print_to_console((60000 / Globals::tapInterval) / 2);
        Globals::print_to_console(" bpm (");
        Globals::print_to_console(Globals::tapInterval);
        Globals::println_to_console(" ms interval in 8th-notes)");
      }

      tapState = 1;
    }

    if (timeSinceFirstTap > 2000) // forget tap if time was too long
    {
      tapState = 1;
      // Globals::println_to_console(("too long...");
    }
    // }
    break;
  }
}

// -------------------- SOUND SWELL: RECORD STROKES -------------------
// --------------------------------------------------------------------
/* SOUND SWELL ALGORITHM: ---------------------------------------------

  RECORD:

  -------X-------X-------X-------X------- hits
  -------|-------|-------|-------|-------
  -------1-------2-------3-------4------- num_of_swell_taps
  -------|-------|-------|-------|-------
  ------502-----510-----518-----526------ beatCount (beatPos)
  -------|-------|-------|-------|-------
  -------|-------|------prev----curr-----
  -------|-------|-------|-------|-------
  -------|-------8-------16------32------ swell_beatPos_sum = sum + (curr - prev)
  -------|---8---|---8---|---8---|------- swell_stroke_interval = sum/num
  ------v++-----v++-----v++-----v++------ increase swell_val


  PLAY:

  -------|-------|-------|-------|-------
  -------0-------1-------2-------3------- beatStep
  -------|-------|-------|-------|-------
  -------v-----(v↓)----(v↓)----(v↓)------ decrease swell_val
  -------🎵-------🎵-------🎵------🎵------- play MIDI note

  ---------------------------------------------------------------------*/
void Instrument::swell_rec(midi::MidiInterface<HardwareSerial> MIDI) // remembers beat stroke position
{
  /* works pretty much just like the tapTempo, but repeats the triggered drums on external MIDI instrument (-> swell_beat() in TIMED INTERVALS) */
  setInstrumentPrintString();

  static unsigned long previous_swell_beatPos;
  // static unsigned long lastSwellRec = 0;

  if (score.swell_state == 1) // first hit
  {
    // grab the current beat counter
    noInterrupts();
    previous_swell_beatPos = Globals::beatCount;
    interrupts();

    // start MIDI note and proceed to next state
    score.swell_state = 2;
    // MIDI.sendNoteOn(midi_settings.active_note, 127, midi_settings.synth);

    // lastSwellRec = millis();
  }

  else if (score.swell_state == 2) // second hit
  {
    if (!Globals::footswitch_is_pressed)
    {
      score.num_of_swell_taps++;
      score.swell_val += 4;                        // ATTENTION: must rise faster than it decreases! otherwise swell resets right away.
      score.swell_val = min(score.swell_val, 127); // max swell_val = 127
    }

    unsigned long current_swell_beatPos;

    noInterrupts();
    current_swell_beatPos = Globals::beatCount;
    interrupts();

    // calculate diff and interval:
    if ((current_swell_beatPos - previous_swell_beatPos) <= 32) // only do this if interval was not too long
    {
      // get diff to first (how many beats were in between?):
      score.swell_beatPos_sum += (current_swell_beatPos - previous_swell_beatPos);
      // average all hits and repeat at that rate:
      float a = float(score.swell_beatPos_sum) / float(score.num_of_swell_taps);
      a += 0.5;                             // rounds up or down
      score.swell_stroke_interval = int(a); // round down
      previous_swell_beatPos = current_swell_beatPos;
    }
  }

  // --------------------------------------------------------------------
}

// increase slot position of current 16th beat when instrument hit:
void Instrument::countup_topography()
{
  if (Globals::printStrokes)
  {
    setInstrumentPrintString();
  }
  topography.a_16[Globals::current_16th_count]++; // will be translated to topography.a_8 when evoked by tsunamiPlayback(?)
}

// increase note_idx of Score class:
void Instrument::mainNoteIteration(Synthesizer *synth_, midi::MidiInterface<HardwareSerial> MIDI, Score *active_score)
{
  static unsigned long lastNoteChange = 0;

  if (millis() > lastNoteChange + 4000) // only do this once in an interval of 4 seconds, because there are always many hits on a cymbal..
  {
    synth_->sendNoteOff(active_score->notes[active_score->note_idx], MIDI);             // turn previous note off
    active_score->note_idx = (active_score->note_idx + 1) % active_score->notes.size(); // iterate note pointer
    synth_->sendNoteOn(active_score->notes[active_score->note_idx], MIDI);              // turn next note on

    lastNoteChange = millis();
  }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////

void Instrument::sendMidiNotes_timed(midi::MidiInterface<HardwareSerial> MIDI)
{
  if (Globals::current_eighth_count != Globals::last_eighth_count) // in 8th-interval
  {
    if (score.read_rhythm_slot[Globals::current_eighth_count])
    {
      setInstrumentPrintString();
      // TODO: SHOULD BE HANDLED LIKE FOOTSWITCHLOOPER!
      midi_settings.synth->sendNoteOn(midi_settings.active_note, MIDI);
    }
    else
      midi_settings.synth->sendNoteOff(midi_settings.active_note, MIDI);
  }
}

void Instrument::setInstrumentSlots()
{
  // set rhythm slots to play MIDI notes:
  score.read_rhythm_slot[Globals::current_eighth_count] = score.set_rhythm_slot[Globals::current_eighth_count];
}

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void Instrument::swell_perform(midi::MidiInterface<HardwareSerial> MIDI) // updates once a 32nd-beat-step
{
  if (score.swell_state == 2)
  {
    // ready to play MIDI again?
    // increase swell_beatStep and modulo interval
    score.swell_beatStep = (score.swell_beatStep + 1) % score.swell_stroke_interval;

    if (score.swell_beatStep == 0) // on swell beat
    {

      // Debug print:
      //output_string[instr] = String(swell_val[instr]);
      //      output_string[instr] = "[";
      //      output_string[instr] += String(swell_stroke_interval[instr]);
      //      output_string[instr] += "] ";
      //      output_string[instr] += num_of_swell_taps[instr];
      //      output_string[instr] += "/";
      //      output_string[instr] += swell_beatPos_sum[instr];
      //      output_string[instr] += " (";
      output_string = String(score.swell_val);
      //      output_string[instr] += ") ";
      output_string += "\t";

      if (!Globals::footswitch_is_pressed)
        midi_settings.synth->sendControlChange(midi_settings.cc_chan, score.swell_val * score.swell_factor, MIDI);

      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */

      // MIDI.sendNoteOn(notes_list[instr], 127, midi_settings.synth); // also play a note on each hit?
      if (effect == CymbalSwell)
      {
        if (Globals::tsunami.isTrackPlaying(score.tsunami_track))
        {
          static int trackLevel = 0;
          static int previousTracklevel = 0;
          trackLevel = min(-40 + score.swell_val, 0);
          if (trackLevel != previousTracklevel)
            Globals::tsunami.trackFade(score.tsunami_track, trackLevel, 100, false); // fade smoothly within 100 ms
          previousTracklevel = trackLevel;
        }
      }
      // decrease swell_val:
      if (score.swell_val > 0)
      {
        if (!Globals::footswitch_is_pressed) // do not decrease while footswitch is pressed
          score.swell_val--;
      }
      else // reset swell if swell_val == 0:
      {
        score.swell_state = 1; // waits for first tap
        score.num_of_swell_taps = 0;
        score.swell_val = 10;
        score.swell_beatPos_sum = 0;
        score.swell_beatStep = 0;
        midi_settings.synth->sendNoteOff(midi_settings.active_note, MIDI);
      }
    }
  }
}
// --------------------------------------------------------------------

//////////////////// TSUNAMI BEAT-LINKED PLAYBACK /////////////////////
///////////////////////////////////////////////////////////////////////

/* ------------- Tsunami Beat-linked Playback algorithm: --------------


  X-----------X---X--------------- 32nd Beat
  [1,  0,  0,  1,  1,  0,  0,  0  ] 1. iteration
  |-----------|---|---------------
  +1, +0, +0, +1, +1, +0, +0, +0    changes between iterations
  |-----------|---|---------------
  [2,  0,  0,  2,  2,  0,  0,  0  ] 2. iteration
  |-----------|---|---------------
  +0. +1, +0, +1, +1, +0, +0, +0    changes, unprecisely played
  ----|-------|---|---------------
  [2,  1,  0,  3,  3,  0,  0,  0  ] 3. iteration
  |-----------|---|---------------
  4 + 1 + 0 + 9 + 9 + 0 + 0 + 0 = 23 beat_topo_squared_sum
  |-----------|---|---------------

  4/23 = 0.174
          ratio = 4:1 = 0.25
  1/23 = 0.043
          ratio = 9:1 = 0.111 --> beat_topo_entries -= 1; beat_topo[i] = 0
  9/23 = 0.391

  beat_topo_regular_sum = 2 + 1 + 3 + 3 = 9
  smoothing: beat_topo_entries = 4 - 1 = 3
  beat_topo_average_smooth = int(9/3 + 0.5) = 3

  ---------------------------------------------------------------------*/

// TODO: also make this a Score function applying to the overall_beat
// matches the instrument's topo to a sound sample stored on the tsunami
void Instrument::tsunamiLink()
{
  // smoothen 16-bit topography to erase sites with noise:
  topography.smoothen_dataArray();

  // translate 16-bit to 8-bit topography for track footprint:
  int j = 0;
  for (int i = 1; i < 16; i += 2)
  {
    if (i > 0 || i - 1 > 0)
    {
      topography.a_8[j] = 1;
    }
    j++;
  }

  // TODO: reduce all params if not played for long.

  int tracknum = 0;                // Debug
  if (topography.regular_sum >= 3) // only initiate playback if average of entries > certain threshold.
  {

    // find right track from database:
    //int tracknum = 0;
    for (int j = 0; j < 8; j++)
    {
      if (topography.a_8[j] > 0)
      {
        if (j == 0)
          tracknum += 128;
        if (j == 1)
          tracknum += 64;
        if (j == 2)
          tracknum += 32;
        if (j == 3)
          tracknum += 16;
        if (j == 4)
          tracknum += 8;
        if (j == 5)
          tracknum += 4;
        if (j == 6)
          tracknum += 2;
        if (j == 7)
          tracknum += 1;
      }
    }
    score.tsunami_track = tracknum; // save for use in other functions

    // set loudness and fade:
    //int trackLevel = min(-40 + (topography.average_smooth * 5), 0);
    int trackLevel = 0;                                                            // Debug
    Globals::tsunami.trackFade(tracknum, trackLevel, Globals::tapInterval, false); // fade smoothly within length of a quarter note

    // TODO: set track channels for each instrument according to output
    // output A: speaker on drumset
    // output B: speaker in room (PA)
    // cool effects: let sounds walk through room from drumset to PA

    // --------------------------- play track -------------------------
    if (!Globals::tsunami.isTrackPlaying(tracknum) && Globals::beatCount == 0)
    {
      // set playback speed according to current_BPM:
      int sr_offset;
      float r = Globals::current_BPM / float(Globals::track_bpm[tracknum]);
      Globals::print_to_console("r = ");
      Globals::println_to_console(r);
      if (!(r > 2) && !(r < 0.5))
      {
        // samplerateOffset scales playback speeds from 0.5 to 1 to 2
        // and maps to -32768 to 0 to 32767
        sr_offset = (r >= 1) ? 32767 * (r - 1) : -32768 + 32768 * 2 * (r - 0.5);
        sr_offset = int(sr_offset);
        Globals::print_to_console("sr_offset = ");
        Globals::println_to_console(sr_offset);
      }
      else
      {
        sr_offset = 0;
      }

      //int channel = 0;                              // Debug
      Globals::tsunami.samplerateOffset(score.tsunami_channel, sr_offset); // TODO: link channels to instruments
      Globals::tsunami.trackGain(tracknum, trackLevel);
      Globals::tsunami.trackPlayPoly(tracknum, score.tsunami_channel, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
      Globals::print_to_console("starting to play track ");
      Globals::println_to_console(tracknum);
    } // track playing end
  }   // threshold end

  // Debug:
  Globals::print_to_console("[");
  for (int i = 0; i < 8; i++)
  {
    Globals::print_to_console(topography.a_8[i]);
    if (i < 7)
      Globals::print_to_console(", ");
  }
  Globals::print_to_console("]\t");
  //  Globals::print_to_console(beat_topo_entries);
  //  Globals::print_to_console("\t");
  //  Globals::print_to_console(beat_topo_squared_sum);
  //  Globals::print_to_console("\t");
  //  Globals::print_to_console(beat_topo_regular_sum);
  //  Globals::print_to_console("\t");
  //  Globals::print_to_console(topography.average_smooth);
  Globals::print_to_console("\t");
  int trackLevel = min(-40 + (topography.average_smooth * 5), 0);
  Globals::print_to_console(trackLevel);
  Globals::print_to_console("dB\t->");
  Globals::println_to_console(tracknum);
}
// --------------------------------------------------------------------

// TODO: make this a static function of Score
// ---------- MIDI playback according to beat_topography --------
void Instrument::topography_midi_effects(std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial> MIDI, Score *active_score)
{
  if (Globals::current_16th_count != Globals::last_16th_count) // do this only once per 16th step
  {
    // smoothen array:
    topography.smoothen_dataArray(); // erases "noise" from arrays if SNR>snr_threshold

    // reset slot for volume
    active_score->topo_midi_effect.reset();

    // sum up all topographies of all instruments:
    for (Instrument *instr : instruments) // of each instrument
    {
      if (instr->effect == TopographyMidiEffect)
        active_score->topo_midi_effect.add(&topography);
    }
    active_score->topo_midi_effect.smoothen_dataArray();

    // ------------ result-> change volume and play MIDI --------
    // ----------------------------------------------------------
    int vol = min(active_score->topo_midi_effect.a_16[Globals::current_16th_count] * 13, 255);

    if (topography.a_16[Globals::current_16th_count] > 0)
      midi_settings.synth->sendControlChange(midi_settings.cc_chan, vol, MIDI);

    // Debug:
    Globals::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
    Globals::printTopoArray(&topography);
    // Globals::print_to_console(":\t[");
    // for (int j = 0; j < 16; j++)
    // {
    //   Globals::print_to_console(topography.a_16[j]);
    //   if (j < 15)
    //     Globals::print_to_console(",");
    // }
    // Globals::println_to_console("]");
  } // end only once per 16th-step
}

/////////////////////////// TIDY UP FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////////////////
/* destructor for playing MIDI notes etc */

void Instrument::turnMidiNoteOff(midi::MidiInterface<HardwareSerial> MIDI)
{
  if (millis() > score.last_notePlayed + 200) // Swell effect turns notes off itself
  {
    midi_settings.synth->sendNoteOff(midi_settings.active_note, MIDI);
    // Globals::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
    // Globals::print_to_console(": turning midi note ");
    // Globals::print_to_console(midi_settings.active_note);
    // Globals::println_to_console(" off.");
  }
}

// TODO: make this decrease with 32nd-notes.
void Instrument::change_cc_out(midi::MidiInterface<HardwareSerial> MIDI) // changes (mostly decreases) value of CC effect each loop (!)
{
  midi_settings.cc_val += midi_settings.cc_tidyUp_factor;
  midi_settings.cc_val = min(max(midi_settings.cc_val, midi_settings.cc_min), midi_settings.cc_max);
  midi_settings.synth->sendControlChange(midi_settings.cc_chan, int(min(midi_settings.cc_val, 127)), MIDI);
  output_string = String(midi_settings.cc_val);
  output_string += "\t";
}

void Instrument::shuffle_cc()
{
  if (score.ready_to_shuffle)
  {
    if (midi_settings.cc_val == midi_settings.cc_standard)
    {

      // ATTENTION:
      // IMPORTANT:
      // TODO: define all CC channels, otherwise this procedure can take forever in the while loop!

      do
      {
        midi_settings.random_cc_chan = int(random(127));
        midi_settings.cc_chan = Globals::int_to_cc_type(midi_settings.random_cc_chan);
      } while (midi_settings.cc_chan == None);

      score.ready_to_shuffle = false;
      Globals::print_to_console("cc_chan of ");
      Globals::print_to_console(drumtype);
      Globals::print_to_console(" is ");
      Globals::println_to_console(midi_settings.cc_chan);
    }
    else
    {
      Globals::print_to_console("waiting for cc_val to be at standard: ");
      Globals::print_to_console(midi_settings.cc_val);
      Globals::print_to_console("/");
      Globals::println_to_console(midi_settings.cc_standard);
    }
  }
}

// instantiate instruments:
Instrument *Drumset::snare = new Instrument(A5, Snare);
Instrument *Drumset::hihat = new Instrument(A6, Hihat);
Instrument *Drumset::kick = new Instrument(A1, Kick);
Instrument *Drumset::tom2 = new Instrument(A7, Tom2);
Instrument *Drumset::standtom = new Instrument(A2, Standtom1);
Instrument *Drumset::cowbell = new Instrument(A3, Cowbell);
Instrument *Drumset::crash1 = new Instrument(A0, Crash1);
Instrument *Drumset::ride = new Instrument(A4, Ride);