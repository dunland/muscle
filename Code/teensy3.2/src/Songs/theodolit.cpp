#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_theodolit(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (get_setup_state())
        {
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY 5TH → UP OCT
            
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}