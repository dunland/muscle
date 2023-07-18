#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_b_63(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(106, MIDI);
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI); // Apreggiator on
            Drumset::hihat->set_effect(TapTempo);
            for (Instrument *instrument : Drumset::instruments)
            {
            //    instrument->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, random(100), 0.05);
                instrument->set_effect(Random_CC_Effect);
                // instrument->shuffle_cc(true);
            }

            Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);
        }
        Hardware::lcd->setCursor(0,0);
        Hardware::lcd->print("simpleARP");
        break;

    default: // just restart
        Globals::active_song->setup_state = true;
        Globals::active_song->step = 0;
        break;
    }
}