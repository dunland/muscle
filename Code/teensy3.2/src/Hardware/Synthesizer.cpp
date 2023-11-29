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

void Synthesizer::mKorg_arpOnOff(byte value) {

  /* http://www.somascape.org/midi/tech/spec.html#rpns
  mKORG ARP On/Off (Manual Seite 59): [Bn, 63, 00, Bn, 62, 02, Bn, 06, mm]
  */

  // NRPN Befehlskette senden
  byte statusByte = 0xB0 | (midi_channel - 1);  // Befehlsbyte für NRPN (0xBn, wobei n der Kanal ist)

  // Senden des NRPN-Befehls
  /* 1. Geben Sie zunächst an, welcher Parameter angesteuert werden soll. Verwenden Sie hierfür NRPN MSB (CC#99) [Bn, 63, mm] und NRPN LSB (CC#98) [Bn, 62, rr] (n: Kanal, mm, rr: Parameternummer; zwei Bytes). */
  MIDI.sendControlChange(99, 00 >> 7, midi_channel);  // MSB (Most Significant Byte) der Parameternummer senden (63 == 0x63 == 99)

  /* 2. Stellen Sie mit CC#06 (MSB) [Bn, 06, mm] den Wert für diesen Parameter ein (n: Kanal, mm: Wert).*/
  MIDI.sendControlChange(98, 02 & 0x7F, midi_channel);  // LSB (Least Significant Byte) der Parameternummer senden (62 == 0x62 == 98)

  MIDI.sendControlChange(6, value >> 7, midi_channel);  // MSB des Wertes senden (Der microKORG wertet nur die CC06-Dateneingabe (MSB) aus)
  // MIDI.sendControlChange(38, value & 0x7F, midi_channel);  // LSB des Wertes senden
}

void Synthesizer::sendProgramChange(int number)
{
  MIDI.sendProgramChange(number, midi_channel);
}