#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// A.72 /////////////////////////////
void run_wueste(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
        Globals::active_song->notes.list = {Note_D3, Note_F4, Note_G4, Note_A4, Note_C5, Note_D6};

            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 13, MIDI);
            Synthesizers::dd200->sendProgramChange(1, MIDI);  // Program #1
            Hardware::footswitch_mode = Increment_Score;
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}