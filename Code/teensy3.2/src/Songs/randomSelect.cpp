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

void run_randomSelect(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int sel_song_int;

    if (Globals::active_song->get_setup_state())
    {
        sel_song_int = list_of_songs.at(random(sizeof(list_of_songs)));
    }

    switch (sel_song_int)
    {
    case 85:
        Globals::active_song = Globals::get_song("b_36");
        // run_b_36(MIDI);
        // Globals::active_song->name = "b_36";
        // TODO: Globals::active_song = *song; // SONST WIRD SONG::SETUP NICHT AUSGEFÜHRT!
        break;

    case 64:
        // run_b_11(MIDI);
        // Globals::active_song->name = "b_11";
        Globals::active_song = Globals::get_song("b_11");
        break;

    case 114:
        Globals::active_song = Globals::get_song("b_73");
        break;

    case 27:
        Globals::active_song = Globals::get_song("b_27");
        break;

    case 106:
        Globals::active_song = Globals::get_song("b_63");
        break;

    case 4:
        Globals::active_song = Globals::get_song("A_15");
        break;

    case 12:
        Globals::active_song = Globals::get_song("A_25");
        break;

    default:
        Devtools::print_to_console("No Song at ");
        Devtools::println_to_console(sel_song_int);
        break;
    }
}
