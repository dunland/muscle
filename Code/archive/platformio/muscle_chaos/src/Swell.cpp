#include <Arduino.h>
#include <Globals.h>
#include <Swell.h>
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

// swell initialization
void Swell::init() // initialize arrays for swell functions (run in setup())
{
  for (int i = 0; i < Globals::numInputs; i++)
  {
    swell_state[i] = 1; // waits for first tap
    num_of_swell_taps[i] = 0;
    swell_val[i] = 10;
    swell_beatPos_sum[i] = 0;
    swell_beatStep[i] = 0;
  }
}

// ---------------------------- RECORD STROKES ------------------------
void Swell::rec(int instr) // remembers beat stroke position
{
  /* works pretty much just like the tapTempo, but repeats the triggered drums on external MIDI instrument (-> swell_beat() in TIMED INTERVALS) */
  static unsigned long previous_swell_beatPos[Globals::numInputs];
  // static unsigned long lastSwellRec = 0;

  if (swell_state[instr] == 1) // first hit
  {
    // grab the current beat counter
    noInterrupts();
    previous_swell_beatPos[instr] = Timing::beatCount;
    interrupts();

    // start MIDI note and proceed to next state
    swell_state[instr] = 2;
    MIDI.sendNoteOn(Score::notes_list[instr], 127, 2);

    //lastSwellRec = millis();
  }

  else if (swell_state[instr] == 2) // second hit
  {
    if (!Hardware::footswitch_is_pressed)
    {
      num_of_swell_taps[instr]++;
      swell_val[instr] += 4;                         // ATTENTION: must rise faster than it decreases! otherwise swell resets right away.
      swell_val[instr] = min(swell_val[instr], 127); // max swell_val = 127
    }

    unsigned long current_swell_beatPos;

    noInterrupts();
    current_swell_beatPos = Timing::beatCount;
    interrupts();

    // calculate diff and interval:
    if ((current_swell_beatPos - previous_swell_beatPos[instr]) <= 32) // only do this if interval was not too long
    {
      // get diff to first (how many beats were in between?):
      swell_beatPos_sum[instr] += (current_swell_beatPos - previous_swell_beatPos[instr]);
      // average all hits and repeat at that rate:
      float a = float(swell_beatPos_sum[instr]) / float(num_of_swell_taps[instr]);
      a += 0.5;                              // rounds up or down
      swell_stroke_interval[instr] = int(a); // round down
      previous_swell_beatPos[instr] = current_swell_beatPos;
    }
  }
}

// --------------------------- REPLAY STROKES -------------------------
void Swell::perform(int instr, int perform_action) // updates once a 32nd-beat-step
{
  if (swell_state[instr] == 2)
  {
    // remember moment of update in beat:
    // noInterrupts();
    // unsigned long updateMoment = Timing::beatCount;
    // interrupts();

    // ready to play MIDI again?
    // increase swell_beatStep and modulo interval
    swell_beatStep[instr] = (swell_beatStep[instr] + 1) % swell_stroke_interval[instr];

    if (swell_beatStep[instr] == 0) // on swell beat
    {

      // Debug print:
      //Debug::output_string[instr] = String(swell_val[instr]);
      //      Debug::output_string[instr] = "[";
      //      Debug::output_string[instr] += String(swell_stroke_interval[instr]);
      //      Debug::output_string[instr] += "] ";
      //      Debug::output_string[instr] += num_of_swell_taps[instr];
      //      Debug::output_string[instr] += "/";
      //      Debug::output_string[instr] += swell_beatPos_sum[instr];
      //      Debug::output_string[instr] += " (";
      Debug::output_string[instr] = String(swell_val[instr]);
      //      Debug::output_string[instr] += ") ";
      Debug::output_string[instr] += "\t";

      if (perform_action == 5)
      {
        if (!Hardware::footswitch_is_pressed)
          MIDI.sendControlChange(Score::cc_chan[instr], swell_val[instr], 2);
      }
      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */

      // MIDI.sendNoteOn(notes_list[instr], 127, 2); // also play a note on each hit?
      else if (perform_action == 7)
      {
        if (Hardware::tsunami.isTrackPlaying(Score::allocated_track[instr]))
        {
          static int trackLevel = 0;
          static int previousTracklevel = 0;
          trackLevel = min(-40 + swell_val[instr], 0);
          if (trackLevel != previousTracklevel)
            Hardware::tsunami.trackFade(Score::allocated_track[instr], trackLevel, 100, false); // fade smoothly within 100 ms
          previousTracklevel = trackLevel;
        }
      }
      // decrease swell_val:
      if (swell_val[instr] > 0)
      {
        if (!Hardware::footswitch_is_pressed)
          swell_val[instr]--;
      }
      else // reset swell if swell_val == 0:
      {
        swell_state[instr] = 1; // waits for first tap
        num_of_swell_taps[instr] = 0;
        swell_val[instr] = 10;
        swell_beatPos_sum[instr] = 0;
        swell_beatStep[instr] = 0;
        MIDI.sendNoteOff(Score::notes_list[instr], 127, 2);
        MIDI.sendNoteOff(1, 2, 3);
      }
    }
  }
}