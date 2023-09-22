#include <Song.h>

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

void run_randomSelect()
{
    static int sel_song_int = 0;
    static Song *randomSong;

    if (Globals::active_song->get_setup_state())
    {
        sel_song_int = list_of_songs.at(int(random(list_of_songs.size() - 1)));
        if (Globals::active_song != randomSong)
            {
                delete randomSong;
                randomSong = nullptr;
            }
    }

    switch (sel_song_int)
    {
    case 85:
        randomSong = new Song(run_b_36, "b_36");
        Globals::active_song = randomSong;
        break;

    case 64:
        randomSong = new Song(run_b_11, "b_11");
        Globals::active_song = randomSong;
        break;

    case 114:
        randomSong = new Song(run_b_73, "b_73");
        Globals::active_song = randomSong;
        break;

    case 27:
        randomSong = new Song(run_b_27, "b_27");
        Globals::active_song = randomSong;
        break;

    case 106:
        randomSong = new Song(run_b_63, "b_63");
        Globals::active_song = randomSong;
        break;

    case 4:
        randomSong = new Song(run_A_15, "A_15");
        Globals::active_song = randomSong;
        break;

    case 12:
        randomSong = new Song(run_A_25, "A_25");
        Globals::active_song = randomSong;
        break;

    default:
        Devtools::print_to_console("No Song at ");
        Devtools::println_to_console(sel_song_int);
        break;
    }

    // static int sel_song_int;

    // switch (Globals::active_song->step)
    // {
    // case 0:
    //     if (Globals::active_song->get_setup_state())
    //     {
    //         sel_song_int = list_of_songs.at(random(sizeof(list_of_songs)));
    //     }

    //     switch (sel_song_int)
    //     {
    //     case 85:
    //         // run_b_36();
    //         // Globals::active_song->name = "b_36";
    //         // TODO: Globals::active_song = *song; // SONST WIRD SONG::SETUP NICHT AUSGEFÜHRT!
    //         break;

    //     case 64:
    //         run_b_11();
    //         Globals::active_song->name = "b_11";
    //         break;

    //     case 114:
    //         run_b_73();
    //         Globals::active_song->name = "b_73";
    //         break;

    //     case 27:
    //         run_b_27();
    //         Globals::active_song->name = "b_27";
    //         break;

    //     case 106:
    //         run_b_63();
    //         Globals::active_song->name = "b_63";
    //         break;

    //     case 4:
    //         run_A_15();
    //         Globals::active_song->name = "A_15";
    //         break;

    //     case 12:
    //         run_A_25();
    //         Globals::active_song->name = "A_25";
    //         break;

    //     default:
    //         Serial.print("No Song at ");
    //         Serial.println(sel_song_int);
    //         break;
    //     }

    //     break;

    // default:
    //     break;
    // }
}
