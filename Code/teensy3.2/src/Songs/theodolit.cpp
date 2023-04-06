#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_theodolit(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Song::step)
    {
    case 0:
        if (Song::setup)
        {
            Synthesizers::whammy->sendProgramChange(58, MIDI); // HARMONY 5TH → UP OCT
            Song::setup = false;
        }
        break;

    default:
        Song::proceed_to_next_score();
        break;
    }
}