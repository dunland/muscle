#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>

//////////////////////// SYNTHESIZER CLASS ////////////////////////////
///////////////////////////////////////////////////////////////////////

// sets cc_value (for JSON communication) and sends MIDI-ControlChange:
void Synthesizer::sendControlChange(CC_Type cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{
  static int previous_val = -1;
  midi_values[cc_type] = val; // store value

  if (cc_type < 0)
  {
    Devtools::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
  }
  else if (val != previous_val)
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
    previous_val = val;
  }
}

void Synthesizer::sendControlChange(int cc_type, int val, midi::MidiInterface<HardwareSerial> MIDI)
{
midi_values[cc_type] = val;

  if (cc_type < 0)
  {
    Devtools::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
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

void Synthesizer::sendProgramChange(int number, midi::MidiInterface<HardwareSerial> MIDI)
{
  MIDI.sendProgramChange(number, midi_channel);
}