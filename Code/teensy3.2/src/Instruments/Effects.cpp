#include <Devtools.h>
#include <Instruments.h>

// #include <Tsunami.h>
#include <Song.h>
#include <Hardware.h>

///////////////////////////////////////////////////////////////////////
/////////////////////////// INSTRUMENT EFFECTS ////////////////////////
///////////////////////////////////////////////////////////////////////

///////////////////////////// TRIGGER EFFECTS /////////////////////////

void Instrument::change_cc_in() // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
{
  midi.cc_val += midi.cc_increase_factor;
  midi.cc_val = min(midi.cc_val, midi.cc_max);
  midi.synth->sendControlChange(midi.cc_chan, int(min(midi.cc_val, 127)));
  output_string = String(midi.cc_val);
  output_string += "\t";
}

void Instrument::random_change_cc_in() // instead of stroke detection, MIDI CC val is altered when sensitivity threshold is crossed.
{
  midi.cc_val += midi.cc_increase_factor;
  midi.cc_val = min(midi.cc_val, midi.cc_max);
  midi.synth->sendControlChange(midi.random_cc_chan, int(min(midi.cc_val, 127)));
  output_string = String(midi.cc_val);
  output_string += "\t";
}

// plays a Midi note:
void Instrument::playMidi()
{
  midi.synth->sendNoteOn(midi.active_note);
  score.last_notePlayed = millis();
}

void Instrument::monitor() // just prints what is being played.
{
  if (Devtools::printStrokes)
  {
    setInstrumentPrintString();
  }
}

void Instrument::toggleRhythmSlot()
{
  if (Devtools::printStrokes)
    setInstrumentPrintString();
  score.read_rhythm_slot[Globals::current_eighth_count] = !score.read_rhythm_slot[Globals::current_eighth_count];
}

void Instrument::footswitch_recordSlots() // record what is being played and replay it later
{
  if (Devtools::printStrokes)
    setInstrumentPrintString();
  score.set_rhythm_slot[Globals::current_eighth_count] = true;
}

// ------------------------------ TAP TEMPO ---------------------------
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

  case 1: // first hit

    if (millis() > timeSinceFirstTap + Globals::active_song->tempo.tapTempoResetTime) // reinitiate tap if not used for ten seconds
    {
      num_of_taps = 0;
      clock_sum = 0;
      Devtools::println_to_console("-----------TAP RESET!-----------\n");
    }
    timeSinceFirstTap = millis(); // record time of first hit
    tapState = 2;                 // next: wait for second hit

    break;

  case 2: // second hit

    if (millis() < timeSinceFirstTap + Globals::active_song->tempo.tapTempoTimeOut) // only record tap if interval was not too long
    {
      num_of_taps++;
      clock_sum += millis() - timeSinceFirstTap;
      Globals::tapInterval = clock_sum / num_of_taps;
      Devtools::print_to_console("new tap Tempo is ");

      // check if score tempo was set, else define quarters are > 300 ms (< 200 BPM)
      // int quarter_timing = (Globals::active_song->tempo.min_tempo > 0) ? Globals::active_song->tempo.min_tempo : 300;

      // TODO: re-apply tap-tempo quarter vs eighth notes differentiation
      //  if (Globals::tapInterval >= quarter_timing && Globals::tapInterval <= 60000 / Globals::active_song->tempo.max_tempo) // quarter notes when slow tapping; tapInterval must be within score tempo range
      if (Globals::tapInterval >= 300)
      {
        Globals::current_BPM = 60000 / Globals::tapInterval;
        Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        Devtools::print_to_console(60000 / Globals::tapInterval);
        Devtools::print_to_console(" bpm (");
        Devtools::print_to_console(Globals::tapInterval);
        Devtools::println_to_console(" ms interval: quarter-notes)");
      }
      // else if (Globals::tapInterval >= 60000 / (Globals::active_song->tempo.min_tempo * 2) && Globals::tapInterval <= 60000 / (Globals::active_song->tempo.max_tempo * 2)) // eighth-notes when fast tapping
      else if (Globals::tapInterval < 300)
      {
        Globals::current_BPM = (60000 / Globals::tapInterval) / 2;                                    // BPM >= 180 → strokes are 8th-notes, BPM half-time
        Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 8 / 128); // 8 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        Devtools::print_to_console((60000 / Globals::tapInterval) / 2);
        Devtools::print_to_console(" bpm (");
        Devtools::print_to_console(Globals::tapInterval);
        Devtools::println_to_console(" ms interval: 8th-notes)");
      }

      tapState = 1;
    }

    if (timeSinceFirstTap > Globals::active_song->tempo.tapTempoTimeOut) // forget tap if time was too long
    {
      tapState = 1;
      // Devtools::println_to_console(("too long...");
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
  ------🎵------🎵-------🎵------🎵------ play MIDI note

  ---------------------------------------------------------------------*/
void Instrument::swell_rec() // remembers beat stroke position
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
    // MIDI.sendNoteOn(midi.active_note, 127, midi.synth);

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
  if (Devtools::printStrokes)
  {
    setInstrumentPrintString();
  }
  topography.a_16[Globals::current_16th_count]++; // will be translated to topography.a_8 when evoked by tsunamiPlayback(?)
}

// increase note_idx of Score class:
void Instrument::mainNoteIteration(Synthesizer *synth_)
{
  static unsigned long lastNoteChange = 0;

  if (millis() > lastNoteChange + 4000) // only do this once in an interval of 4 seconds, because there are always many hits on a cymbal..
  {
    synth_->sendNoteOff(Globals::active_song->notes[Globals::active_song->note_idx]);  // turn previous note off
    Globals::active_song->note_idx = (Globals::active_song->note_idx + 1) % Globals::active_song->notes.size(); // iterate note pointer
    synth_->sendNoteOn(Globals::active_song->notes[Globals::active_song->note_idx]);  // turn next note on

    lastNoteChange = millis();
  }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////

void Instrument::sendMidiNotes_timed()
{
  if (Globals::current_eighth_count != Globals::last_eighth_count) // in 8th-interval
  {
    if (score.read_rhythm_slot[Globals::current_eighth_count])
    {
      setInstrumentPrintString();
      // TODO: SHOULD BE HANDLED LIKE FOOTSWITCHLOOPER!
      midi.synth->sendNoteOn(midi.active_note);
    }
    else
      midi.synth->sendNoteOff(midi.active_note);
  }
}

void Instrument::setInstrumentSlots()
{
  // set rhythm slots to play MIDI notes:
  score.read_rhythm_slot[Globals::current_eighth_count] = score.set_rhythm_slot[Globals::current_eighth_count];
}

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void Instrument::swell_perform() // updates once a 32nd-beat-step
{
  if (score.swell_state == 2)
  {
    // ready to play MIDI again?
    // increase swell_beatStep and modulo interval
    score.swell_beatStep = (score.swell_beatStep + 1) % score.swell_stroke_interval;

    if (score.swell_beatStep == 0) // on swell beat
    {

      // Debug print:
      // output_string[instr] = String(swell_val[instr]);
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
        midi.synth->sendControlChange(midi.cc_chan, score.swell_val * score.swell_factor);

      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */

      // MIDI.sendNoteOn(notes_list[instr], 127, midi.synth); // also play a note on each hit?
      // if (effect == CymbalSwell)
      // {
      //   if (Globals::tsunami.isTrackPlaying(score.tsunami_track))
      //   {
      //     static int trackLevel = 0;
      //     static int previousTracklevel = 0;
      //     trackLevel = min(-40 + score.swell_val, 0);
      //     if (trackLevel != previousTracklevel)
      //       Globals::tsunami.trackFade(score.tsunami_track, trackLevel, 100, false); // fade smoothly within 100 ms
      //     previousTracklevel = trackLevel;
      //   }
      // }
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
        midi.synth->sendNoteOff(midi.active_note);
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
    // int tracknum = 0;
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
    // int trackLevel = min(-40 + (topography.average_smooth * 5), 0);
    // int trackLevel = 0;                                                            // Debug
    // Globals::tsunami.trackFade(tracknum, trackLevel, Globals::tapInterval, false); // fade smoothly within length of a quarter note

    // TODO: set track channels for each instrument according to output
    // output A: speaker on drumset
    // output B: speaker in room (PA)
    // cool effects: let sounds walk through room from drumset to PA

    // --------------------------- play track -------------------------
    // if (!Globals::tsunami.isTrackPlaying(tracknum) && Globals::beatCount == 0)
    // {
    //   // set playback speed according to current_BPM:
    //   int sr_offset;
    //   float r = Globals::current_BPM / float(Globals::track_bpm[tracknum]);
    //   Devtools::print_to_console("r = ");
    //   Devtools::println_to_console(r);
    //   if (!(r > 2) && !(r < 0.5))
    //   {
    //     // samplerateOffset scales playback speeds from 0.5 to 1 to 2
    //     // and maps to -32768 to 0 to 32767
    //     sr_offset = (r >= 1) ? 32767 * (r - 1) : -32768 + 32768 * 2 * (r - 0.5);
    //     sr_offset = int(sr_offset);
    //     Devtools::print_to_console("sr_offset = ");
    //     Devtools::println_to_console(sr_offset);
    //   }
    //   else
    //   {
    //     sr_offset = 0;
    //   }

    //   //int channel = 0;                              // Debug
    //   Globals::tsunami.samplerateOffset(score.tsunami_channel, sr_offset); // TODO: link channels to instruments
    //   Globals::tsunami.trackGain(tracknum, trackLevel);
    //   Globals::tsunami.trackPlayPoly(tracknum, score.tsunami_channel, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
    //   Devtools::print_to_console("starting to play track ");
    //   Devtools::println_to_console(tracknum);
    // } // track playing end
  } // threshold end

  // Debug:
  Devtools::print_to_console("[");
  for (int i = 0; i < 8; i++)
  {
    Devtools::print_to_console(topography.a_8[i]);
    if (i < 7)
      Devtools::print_to_console(", ");
  }
  Devtools::print_to_console("]\t");
  //  Devtools::print_to_console(beat_topo_entries);
  //  Devtools::print_to_console("\t");
  //  Devtools::print_to_console(beat_topo_squared_sum);
  //  Devtools::print_to_console("\t");
  //  Devtools::print_to_console(beat_topo_regular_sum);
  //  Devtools::print_to_console("\t");
  //  Devtools::print_to_console(topography.average_smooth);
  Devtools::print_to_console("\t");
  int trackLevel = min(-40 + (topography.average_smooth * 5), 0);
  Devtools::print_to_console(trackLevel);
  Devtools::print_to_console("dB\t->");
  Devtools::println_to_console(tracknum);
}
// --------------------------------------------------------------------

// TODO: make this a static function of Score
// ---------- MIDI playback according to beat_topography --------
void Instrument::topography_midi_effects(std::vector<Instrument *> instruments)
{
  if (Globals::current_16th_count != Globals::last_16th_count) // do this only once per 16th step
  {
    // smoothen array:
    topography.smoothen_dataArray(); // erases "noise" from arrays if SNR>snr_threshold

    // reset slot for volume
    Globals::active_song->topo_midi_effect.reset();

    // sum up all topographies of all instruments:
    for (Instrument *instr : instruments) // of each instrument
    {
      if (instr->effect == TopographyMidiEffect)
        Globals::active_song->topo_midi_effect.add(&topography);
    }
    Globals::active_song->topo_midi_effect.smoothen_dataArray();

    // ------------ result-> change volume and play MIDI --------
    // ----------------------------------------------------------
    int vol = min(Globals::active_song->topo_midi_effect.a_16[Globals::current_16th_count] * 13, 255);

    if (topography.a_16[Globals::current_16th_count] > 0)
      midi.synth->sendControlChange(midi.cc_chan, vol);

    // Debug:
    Devtools::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
    topography.print();
    // Devtools::print_to_console(":\t[");
    // for (int j = 0; j < 16; j++)
    // {
    //   Devtools::print_to_console(topography.a_16[j]);
    //   if (j < 15)
    //     Devtools::print_to_console(",");
    // }
    // Devtools::println_to_console("]");
  } // end only once per 16th-step
}

/////////////////////////// TIDY UP FUNCTIONS /////////////////////////
///////////////////////////////////////////////////////////////////////
/* destructor for playing MIDI notes etc */

void Instrument::turnMidiNoteOff()
{
  if (millis() > score.last_notePlayed + 200) // Swell effect turns notes off itself
  {
    midi.synth->sendNoteOff(midi.active_note);
    // Devtools::print_to_console(Globals::DrumtypeToHumanreadable(drumtype));
    // Devtools::print_to_console(": turning midi note ");
    // Devtools::print_to_console(midi.active_note);
    // Devtools::println_to_console(" off.");
  }
}

// TODO: make this decrease with 32nd-notes.
void Instrument::change_cc_out() // changes (mostly decreases) value of CC effect each loop (!)
{
  midi.cc_val += midi.cc_tidyUp_factor;
  midi.cc_val = min(max(midi.cc_val, midi.cc_min), midi.cc_max);
  midi.synth->sendControlChange(midi.cc_chan, int(min(midi.cc_val, 127)));
  output_string = String(midi.cc_val);
  output_string += "\t";
}

// sets the midi channel to a random value of all implemented channels
void Instrument::shuffle_cc(boolean force_ = false)
{
  if (score.ready_to_shuffle || force_ == true)
  {
    if (midi.cc_val == midi.cc_standard || force_ == true)
    {

      // ATTENTION:
      // IMPORTANT:
      // TODO: define all CC channels, otherwise this procedure can take forever in the while loop!

      do
      {
        midi.random_cc_chan = int(random(128));
        midi.cc_chan = Globals::int_to_cc_type(midi.random_cc_chan);
      } while (midi.cc_chan == CC_None);

      score.ready_to_shuffle = false;
      Devtools::print_to_console("cc_chan of ");
      Devtools::print_to_console(drumtype);
      Devtools::print_to_console(" is ");
      Devtools::println_to_console(midi.cc_chan);
    }
    else
    {
      Devtools::print_to_console("waiting for cc_val to be at standard: ");
      Devtools::print_to_console(midi.cc_val);
      Devtools::print_to_console("/");
      Devtools::println_to_console(midi.cc_standard);
    }
  }
  else
  {
    Devtools::println_to_console("could not shuffle CC, because !score.ready_to_shuffle");
  }
}