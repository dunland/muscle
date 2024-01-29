#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>
#include <MIDI_Instance.h>

void Hardware::begin_MIDI()
{
  MIDI.begin(MIDI_CHANNEL_OMNI);
  // turn off all currently playing MIDI notes:
  for (int channel = 1; channel < 3; channel++)
  {
    for (int note_number = 0; note_number < 127; note_number++)
    {
      MIDI.sendNoteOff(note_number, 127, channel);
    }
  }
}

void Hardware::sendMidiClock(){
  MIDI.sendRealTime(midi::Clock);
}

//////////////////////// SYNTHESIZER CLASS ////////////////////////////
///////////////////////////////////////////////////////////////////////

// sets cc_value (for JSON communication) and sends MIDI-ControlChange:
void Synthesizer::sendControlChange(CC_Type cc_type, int val)
{
  static int previous_val = -1;
  midi_values[cc_type] = val; // store value

  if (cc_type < 0)
  {
    Devtools::println_to_console("could not send MIDI CC Command: CC_Type not defined.");
    Hardware::lcd->setCursor(15,1);
    Hardware::lcd->print("!");
  }
  else if (val != previous_val)
  {
    MIDI.sendControlChange(int(cc_type), val, midi_channel);
    previous_val = val;
  }
}

void Synthesizer::sendControlChange(int cc_type, int val)
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

void Synthesizer::sendNoteOn(int note)
{
  notes[note] = true; // remember that note is turned on
  MIDI.sendNoteOn(note, 127, midi_channel);
}

void Synthesizer::sendNoteOff(int note)
{
  notes[note] = false; // remember that note is turned off
  MIDI.sendNoteOff(note, 127, midi_channel);
}

void Synthesizer::sendProgramChange(int number)
{
  MIDI.sendProgramChange(number, midi_channel);
}

void Synthesizer::whammyPedal(int value){
  Serial2.write(midi_channel);
  Serial2.write(11);
  Serial2.write(value);
}