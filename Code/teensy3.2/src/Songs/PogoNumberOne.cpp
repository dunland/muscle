#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch(Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Globals::bSendMidiClock = false;

            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT

            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 4, MIDI);
            Synthesizers::dd200->sendProgramChange(3, MIDI);  // Program #3

            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI); // ATTENTION: must be after programChange!
            Hardware::footswitch_mode = Increment_Score;

        }
        break;

    default:
        Globals::bSendMidiClock = true;
        Globals::active_song->proceed_to_next_score();
        break;
    }
}