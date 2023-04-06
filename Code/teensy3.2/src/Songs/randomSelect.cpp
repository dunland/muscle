#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

static std::vector<int> list_of_songs = {0, 1, 2, 3};

void Song::run_randomSelect(midi::MidiInterface<HardwareSerial> MIDI)
{

    switch (step)
    
    {
    case 0:
        if (setup)
        {
            Synthesizers::mKorg->sendProgramChange(list_of_songs.at(random(sizeof(list_of_songs))), MIDI); // get random entry; start random predefined program
            setup = false;
        }
        break;

    case 1:
        if (setup)
        {
        }
    break;

    default:
    break;
    }
}
