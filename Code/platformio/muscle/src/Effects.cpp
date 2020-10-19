#include <Effects.h> // take care this is always on top!
// #include <Instruments.h> // do not include anything here that is also in .h
#include <Globals.h>
#include <MIDI.h>

///////////////////////////// TRIGGER EFFECTS /////////////////////////
///////////////////////////////////////////////////////////////////////

void Effect::playMidi(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
  MIDI.sendNoteOn(instrument->score.active_note, 127, 2);
  instrument->score.last_notePlayed = millis();
}

void Effect::monitor(Instrument *instrument) // just prints what is being played.
{
  if (Globals::printStrokes)
  {
    Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
  }
}

void Effect::toggleRhythmSlot(Instrument *instrument)
{
  if (Globals::printStrokes)
    Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
  instrument->score.read_rhythm_slot[Globals::current_eighth_count] = !instrument->score.read_rhythm_slot[Globals::current_eighth_count];
}

void Effect::footswitch_recordSlots(Instrument *instrument) // record what is being played and replay it later
{
  if (Globals::printStrokes)
    Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
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
      Serial.println("-----------TAP RESET!-----------\n");
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
      Serial.print("new tap Tempo is ");
      Serial.print(60000 / Globals::tapInterval);
      Serial.print(" bpm (");
      Serial.print(Globals::tapInterval);
      Serial.println(" ms interval)");

      Globals::current_BPM = 60000 / Globals::tapInterval;
      tapState = 1;

      Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
    }

    if (timeSinceFirstTap > 2000) // forget tap if time was too long
    {
      tapState = 1;
      // Serial.println("too long...");
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
  Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);

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
    MIDI.sendNoteOn(instrument->score.active_note, 127, 2);

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

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

// -------------------- SOUND SWELL: REPLAY STROKES -------------------
// --------------------------------------------------------------------

void Effect::swell_perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI) // updates once a 32nd-beat-step
{
  if (instrument->score.swell_state == 2)
  {
    // remember moment of update in beat:
    //    noInterrupts();
    //    unsigned long updateMoment = beatCount;
    //    interrupts();

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
      Globals::output_string[instrument->drumtype] = String(instrument->score.swell_val);
      //      output_string[instr] += ") ";
      Globals::output_string[instrument->drumtype] += "\t";

      if (instrument->effect == Swell)
      {
        if (!Globals::footswitch_is_pressed)
          MIDI.sendControlChange(instrument->midi.cc_chan, instrument->score.swell_val, 2);
      }
      /* channels on mKORG: 44=cutoff, 50=amplevel, 23=attack, 25=sustain, 26=release
        finding the right CC# on microKORG: (manual p.61):
        1. press SHIFT + 5
        2. choose parameter to find out via EDIT SELECT 1 & 2
        (3. reset that parameter, if you like) */

      // MIDI.sendNoteOn(notes_list[instr], 127, 2); // also play a note on each hit?
      else if (instrument->effect == CymbalSwell)
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
        if (!Globals::footswitch_is_pressed)
          instrument->score.swell_val--;
      }
      else // reset swell if swell_val == 0:
      {
        instrument->score.swell_state = 1; // waits for first tap
        instrument->score.num_of_swell_taps = 0;
        instrument->score.swell_val = 10;
        instrument->score.swell_beatPos_sum = 0;
        instrument->score.swell_beatStep = 0;
        MIDI.sendNoteOff(instrument->score.active_note, 127, 2);
      }
    }
  }
}
// --------------------------------------------------------------------