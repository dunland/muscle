#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

static std::vector<int> list_of_songs = {
    64,  // b.11 + OSC2->Semitone Scale -24 -> +24
    85,  // b.36 z.B. Kick->Filter-Type
    114, // b.73 = noise-trigger
    27,  // b.27
    106, // b.63 mit ARP + hihat-clock
    4,   // A.15
    12   // A.25
};

void *run_selected_song();

void Song::run_randomSelect(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int sel_song_int;

    switch (step)
    {
    case 0:
        if (setup)
        {
            sel_song_int = list_of_songs.at(random(sizeof(list_of_songs)));
            Synthesizers::mKorg->sendProgramChange(sel_song_int, MIDI); // get random entry; start random predefined program
            setup = false;
        }

        switch (sel_song_int)
        {
        case 85:
            run_b_36(MIDI);
            break;

        case 64:
            run_b_11(MIDI);
            break;

        case 114:
            run_b_73(MIDI);
            break;

        case 27:
            run_b_27(MIDI);
            break;

        case 106:
            run_b_36(MIDI);
            break;

        case 4:
            run_A_15(MIDI);
            break;

        case 12:
            run_A_25(MIDI);
            break;

        default:
            Serial.print("No Song at ");
            Serial.println(sel_song_int);
            break;
        }

        break;

    default:
        break;
    }
}
