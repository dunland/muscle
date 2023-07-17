#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_ferdinandPiech(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::dd200->sendProgramChange(6, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 127, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 19, MIDI); // 84 BPM
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}