#include <Hardware.h>
#include <Score/Score.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>

//////////////////////// SYNTHESIZER CLASS ////////////////////////////
///////////////////////////////////////////////////////////////////////

// sets cc_value (for JSON communication) and sends MIDI-ControlChange:
void Synthesizer::sendControlChange(CC_Type cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{
  switch (cc_type)
  {
  case Osc2_semitone:
    osc2_semitone = val;
    break;

  case Osc2_tune:
    osc2_tune = val;
    break;

  case Mix_Level_1:
    mix_level_1 = val;
    break;

  case Mix_Level_2:
    mix_level_2 = val;
    break;

  case Patch_1_Depth:
    patch1_depth = val;
    break;

  case Patch_3_Depth:
    patch3_depth = val;
    break;

  case Cutoff:
    cutoff = val;
    break;

  case Resonance:
    resonance = val;
    break;

  case Amplevel:
    amplevel = val;
    break;

  case Attack:
    attack = val;
    break;

  case Sustain:
    sustain = val;
    break;

  case Release:
    release = val;
    break;

  case DelayTime:
    delaytime = val;
    break;

  case DelayDepth:
    delaydepth = val;
    break;

  default:
    break;
  }

  if (cc_type < 0)
  {
    Globals::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
  }
  else
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
  }
}

void Synthesizer::sendControlChange(int cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{

  // TODO: complete this list. May lead to incomplete monitoring otherwise..
  switch (cc_type)
  {
  case Osc2_semitone:
    osc2_semitone = val;
    break;

  case Osc2_tune:
    osc2_tune = val;
    break;

  case Mix_Level_1:
    mix_level_1 = val;
    break;

  case Mix_Level_2:
    mix_level_2 = val;
    break;

  case Patch_1_Depth:
    patch1_depth = val;
    break;

  case Patch_3_Depth:
    patch3_depth = val;
    break;

  case Cutoff:
    cutoff = val;
    break;

  case Resonance:
    resonance = val;
    break;

  case Amplevel:
    amplevel = val;
    break;

  case Attack:
    attack = val;
    break;

  case Sustain:
    sustain = val;
    break;

  case Release:
    release = val;
    break;

  case DelayTime:
    delaytime = val;
    break;

  case DelayDepth:
    delaydepth = val;
    break;

  default:
    break;
  }

  if (cc_type < 0)
  {
    Globals::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
  }
  else
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
  }
}

void Synthesizer::sendNoteOn(int note, midi::MidiInterface<HardwareSerial> MIDI)
{
  notes[note] = true; // remember that note is turned on
  MIDI.sendNoteOn(note, 127, midi_channel);
}

void Synthesizer::sendNoteOff(int note, midi::MidiInterface<HardwareSerial> MIDI)
{
  notes[note] = false; // remember that note is turned off
  MIDI.sendNoteOff(note, 127, midi_channel);
}