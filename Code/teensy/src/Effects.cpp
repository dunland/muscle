#include <Effects.h> // take care this is always on top!
// #include <Instruments.h> // do not include anything here that is also in .h
#include <Globals.h>
#include <MIDI.h>

// create overall volume topography of all instrument layers:
TOPOGRAPHY Effect::beat_sum;
// std::vector<int> Effect::beat_sum.a_16 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

///////////////////////////// TRIGGER EFFECTS /////////////////////////
///////////////////////////////////////////////////////////////////////

void Effect::cc_effect_rawPin(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI) // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
{
  // if (abs(instrument->sensitivity.noiseFloor - analogRead(instrument->pin)) > instrument->sensitivity.threshold)
  // {
  instrument->midi.cc_val += instrument->midi.cc_increase_factor;
  instrument->midi.cc_val = min(instrument->midi.cc_val, instrument->midi.cc_max);
  MIDI.sendControlChange(instrument->midi.cc_chan, int(min(instrument->midi.cc_val, 127)), microKORG);
  instrument->output_string = String(instrument->midi.cc_val);
  instrument->output_string += "\t";
  // }
  // else
  // {
  //   instrument->midi.cc_val -= instrument->midi.cc_decay_factor;
  //   instrument->midi.cc_val = max(instrument->midi.cc_val, instrument->midi.cc_min);
  //   MIDI.sendControlChange(instrument->midi.cc_chan, int(min(instrument->midi.cc_val, 127)), 2);
  //   Globals::output_string[instrument->drumtype] = String(int(instrument->midi.cc_val));
  // }
}

void Effect::playMidi_rawPin(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  if (abs(instrument->sensitivity.noiseFloor - analogRead(instrument->pin)) > instrument->sensitivity.threshold)
    MIDI.sendNoteOn(instrument->midi.active_note, 127, 2);
  else
    MIDI.sendNoteOff(instrument->midi.active_note, 127, 2);
}

void Effect::playMidi(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  MIDI.sendNoteOn(instrument->midi.active_note, 127, 2);
  instrument->score.last_notePlayed = millis();
}

void Effect::monitor(Instrument *instrument) // just prints what is being played.
{
  if (Globals::printStrokes)
  {
    instrument->setInstrumentPrintString();
  }
}

void Effect::toggleRhythmSlot(Instrument *instrument)
{
  if (Globals::printStrokes)
    instrument->setInstrumentPrintString();
  instrument->score.read_rhythm_slot[Globals::current_eighth_count] = !instrument->score.read_rhythm_slot[Globals::current_eighth_count];
}

void Effect::footswitch_recordSlots(Instrument *instrument) // record what is being played and replay it later
{
  if (Globals::printStrokes)
    instrument->setInstrumentPrintString();
  instrument->score.set_rhythm_slot[Globals::current_eighth_count] = true;
}

void Effect::getTapTempo()
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

      if (Globals::tapInterval >= 333) // quarter notes when BPM <= 180
      {
        Globals::current_BPM = 60000 / Globals::tapInterval;
        Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        Globals::print_to_console(60000 / Globals::tapInterval);
        Globals::print_to_console(" bpm (");
        Globals::print_to_console(Globals::tapInterval);
        Globals::println_to_console(" ms interval int quarter-notes)");
      }
      else // eighth-notes when BPM >= 180
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
void Effect::swell_rec(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI) // remembers beat stroke position
{
  /* works pretty much just like the tapTempo, but repeats the triggered drums on external MIDI instrument (-> swell_beat() in TIMED INTERVALS) */
  instrument->setInstrumentPrintString();

  static unsigned long previous_swell_beatPos;
  // static unsigned long lastSwellRec = 0;

  if (instrument->score.swell_state == 1) // first hit
  {
    // grab the current beat counter
    noInterrupts();
    previous_swell_beatPos = Globals::beatCount;
    interrupts();

    // start MIDI note and proceed to next state
    instrument->score.swell_state = 2;
    // MIDI.sendNoteOn(instrument->midi.active_note, 127, 2);

    // lastSwellRec = millis();
  }

  else if (instrument->score.swell_state == 2) // second hit
  {
    if (!Globals::footswitch_is_pressed)
    {
      instrument->score.num_of_swell_taps++;
      instrument->score.swell_val += 4;                                    // ATTENTION: must rise faster than it decreases! otherwise swell resets right away.
      instrument->score.swell_val = min(instrument->score.swell_val, 127); // max swell_val = 127
    }

    unsigned long current_swell_beatPos;

    noInterrupts();
    current_swell_beatPos = Globals::beatCount;
    interrupts();

    // calculate diff and interval:
    if ((current_swell_beatPos - previous_swell_beatPos) <= 32) // only do this if interval was not too long
    {
      // get diff to first (how many beats were in between?):
      instrument->score.swell_beatPos_sum += (current_swell_beatPos - previous_swell_beatPos);
      // average all hits and repeat at that rate:
      float a = float(instrument->score.swell_beatPos_sum) / float(instrument->score.num_of_swell_taps);
      a += 0.5;                                         // rounds up or down
      instrument->score.swell_stroke_interval = int(a); // round down
      previous_swell_beatPos = current_swell_beatPos;
    }
  }

  // --------------------------------------------------------------------
}

void Effect::countup_topography(Instrument *instrument) // increases slot position of current 16th beat when instrument hit
{
  if (Globals::printStrokes)
  {
    instrument->setInstrumentPrintString();
  }
  instrument->topography.a_16[Globals::current_16th_count]++; // will be translated to topography.a_8 when evoked by tsunamiPlayback(?)
}

///////////////////////////////////////////////////////////////////////
//////////////////////////////// PERFORM //////////////////////////////
///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

void Effect::sendMidiNotes_timed(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  if (Globals::current_eighth_count != Globals::last_eighth_count) // in 8th-interval
  {
    if (instrument->score.read_rhythm_slot[Globals::current_eighth_count])
    {
      instrument->setInstrumentPrintString();
      // TODO: SHOULD BE HANDLED LIKE FOOTSWITCHLOOPER!
      MIDI.sendNoteOn(instrument->midi.active_note, 127, 2);
    }
    else
      MIDI.sendNoteOff(instrument->midi.active_note, 127, 2);
  }
}

void Effect::setInstrumentSlots(Instrument *instrument)
{
  // set rhythm slots to play MIDI notes:
  instrument->score.read_rhythm_slot[Globals::current_eighth_count] = instrument->score.set_rhythm_slot[Globals::current_eighth_count];
}

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void Effect::swell_perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI) // updates once a 32nd-beat-step
{
  if (instrument->score.swell_state == 2)
  {
    // ready to play MIDI again?
    // increase swell_beatStep and modulo interval
    instrument->score.swell_beatStep = (instrument->score.swell_beatStep + 1) % instrument->score.swell_stroke_interval;

    if (instrument->score.swell_beatStep == 0) // on swell beat
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
      instrument->output_string = String(instrument->score.swell_val);
      //      output_string[instr] += ") ";
      instrument->output_string += "\t";

      if (!Globals::footswitch_is_pressed)
        MIDI.sendControlChange(instrument->midi.cc_chan, instrument->score.swell_val * instrument->score.swell_factor, 2);
      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */

      // MIDI.sendNoteOn(notes_list[instr], 127, 2); // also play a note on each hit?
      if (instrument->effect == CymbalSwell)
      {
        if (Globals::tsunami.isTrackPlaying(instrument->score.allocated_track))
        {
          static int trackLevel = 0;
          static int previousTracklevel = 0;
          trackLevel = min(-40 + instrument->score.swell_val, 0);
          if (trackLevel != previousTracklevel)
            Globals::tsunami.trackFade(instrument->score.allocated_track, trackLevel, 100, false); // fade smoothly within 100 ms
          previousTracklevel = trackLevel;
        }
      }
      // decrease swell_val:
      if (instrument->score.swell_val > 0)
      {
        if (!Globals::footswitch_is_pressed) // do not decrease while footswitch is pressed
          instrument->score.swell_val--;
      }
      else // reset swell if swell_val == 0:
      {
        instrument->score.swell_state = 1; // waits for first tap
        instrument->score.num_of_swell_taps = 0;
        instrument->score.swell_val = 10;
        instrument->score.swell_beatPos_sum = 0;
        instrument->score.swell_beatStep = 0;
        MIDI.sendNoteOff(instrument->midi.active_note, 127, 2);
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

void Effect::tsunami_beat_playback(Instrument *instrument)
{
  // smoothen 16-bit topography to erase sites with noise:
  Globals::smoothen_dataArray(&instrument->topography);

  // translate 16-bit to 8-bit topography for track footprint:
  int j = 0;
  for (int i = 1; i < 16; i += 2)
  {
    if (i > 0 || i - 1 > 0)
    {
      instrument->topography.a_8[j] = 1;
    }
    j++;
  }

  // TODO: reduce all params if not played for long.

  int tracknum = 0;                            // Debug
  if (instrument->topography.regular_sum >= 3) // only initiate playback if average of entries > certain threshold.
  {

    // find right track from database:
    //int tracknum = 0;
    for (int j = 0; j < 8; j++)
    {
      if (instrument->topography.a_8[j] > 0)
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
    instrument->score.allocated_track = tracknum; // save for use in other functions

    // set loudness and fade:
    //int trackLevel = min(-40 + (instrument->topography.average_smooth * 5), 0);
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
      Globals::tsunami.samplerateOffset(instrument->score.allocated_channel, sr_offset); // TODO: link channels to instruments
      Globals::tsunami.trackGain(tracknum, trackLevel);
      Globals::tsunami.trackPlayPoly(tracknum, instrument->score.allocated_channel, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
      Globals::print_to_console("starting to play track ");
      Globals::println_to_console(tracknum);
    } // track playing end
  }   // threshold end

  // Debug:
  Globals::print_to_console("[");
  for (int i = 0; i < 8; i++)
  {
    Globals::print_to_console(instrument->topography.a_8[i]);
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
  //  Globals::print_to_console(instrument->topography.average_smooth);
  Globals::print_to_console("\t");
  int trackLevel = min(-40 + (instrument->topography.average_smooth * 5), 0);
  Globals::print_to_console(trackLevel);
  Globals::print_to_console("dB\t->");
  Globals::println_to_console(tracknum);
}
// --------------------------------------------------------------------

// ---------- MIDI playback according to beat_topography --------
void Effect::topography_midi_effects(Instrument *instrument, std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial> MIDI)
{
  if (Globals::current_16th_count != Globals::last_16th_count) // do this only once per 16th step
  {
    // smoothen array:
    Globals::smoothen_dataArray(&instrument->topography); // erases "noise" from arrays if SNR>snr_threshold

    // reset slot for volume
    beat_sum.a_16 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // sum up all topographies of all instruments:
    for (int idx = 0; idx < 16; idx++) // each slot
    {
      for (Instrument *instr : instruments) // of each instrument
      {
        if (instr->effect == TopographyLog)
          beat_sum.a_16[idx] += instrument->topography.a_16[idx];
      }
    }
    Globals::smoothen_dataArray(&beat_sum);

    // ------------ result-> change volume and play MIDI --------
    // ----------------------------------------------------------
    int vol = min(beat_sum.a_16[Globals::current_16th_count] * 13, 255);

    if (instrument->topography.a_16[Globals::current_16th_count] > 0)
      MIDI.sendControlChange(instrument->midi.cc_chan, vol, 2);

    // Debug:
    Globals::print_to_console(Globals::DrumtypeToHumanreadable(instrument->drumtype));
    Globals::printTopoArray(&instrument->topography);
    // Globals::print_to_console(":\t[");
    // for (int j = 0; j < 16; j++)
    // {
    //   Globals::print_to_console(instrument->topography.a_16[j]);
    //   if (j < 15)
    //     Globals::print_to_console(",");
    // }
    // Globals::println_to_console("]");
  } // end only once per 16th-step
}

/////////////////////////// TIDY UP FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////////////////
/* destructor for playing MIDI notes etc */

void Effect::turnMidiNoteOff(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  if (millis() > instrument->score.last_notePlayed + 200 && instrument->effect != Swell) // Swell effect turns notes off itself
    MIDI.sendNoteOff(instrument->midi.active_note, 127, 2);
}

// TODO: make this decrease with 32nd-notes.
void Effect::decay_ccVal(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI) // decreases value of CC effect each loop (!)
{
  instrument->midi.cc_val -= instrument->midi.cc_decay_factor;
  instrument->midi.cc_val = max(instrument->midi.cc_val, instrument->midi.cc_min);
  MIDI.sendControlChange(instrument->midi.cc_chan, int(min(instrument->midi.cc_val, 127)), 2);
  instrument->output_string = String(instrument->midi.cc_val);
  instrument->output_string += "\t";
}