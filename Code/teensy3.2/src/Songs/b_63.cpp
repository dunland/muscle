#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_b_63(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int randomNote = int(random(16, 72));

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(106, MIDI);
            Globals::bSendMidiClock = true;
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI); // Apreggiator on

            Drumset::hihat->set_effect(TapTempo);
            // for (Instrument *instrument : Drumset::instruments)
            // {
                // TODO: random CC effect for instrument.. FIXME: does not work by just setting Effect to Random_CC_Effect
            // }
        }

        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[randomNote] == false)
            Synthesizers::mKorg->sendNoteOn(randomNote, MIDI);

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("simpleARP");
        break;

    default: // change note
        Synthesizers::mKorg->sendNoteOff(randomNote, MIDI);
        randomNote = 24 + ((randomNote + int(random(1, 13))) % 103);
        Synthesizers::mKorg->sendNoteOn(randomNote, MIDI);
        break;
    }
}