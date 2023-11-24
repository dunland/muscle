#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// ZITTERAAL /////////////////////////////
void run_TEMPLATE()
{
    switch(Globals::active_song->step)
    {
    case 0: // Snare → Vocoder (D+F)
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();
        }

        break;

    case 1:
        if (Globals::active_song->get_setup_state())
        {
        }

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}