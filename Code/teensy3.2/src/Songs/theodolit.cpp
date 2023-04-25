#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_theodolit(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendProgramChange(3, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI); // ATTENTION: must be after programChange!
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 14, MIDI); // 518 BPM

            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY 5TH → UP OCT
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}