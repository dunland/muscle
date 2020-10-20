// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------
#include <Arduino.h>
#include <Globals.h>
#include <Score.h>

int Score::notes_list[] = {60, 61, 45, 74, 72, 44, 71};
int Score::cc_chan[] = {50, 0, 0, 25, 71, 50, 0, 0}; // needed in pinAction 5 and 6

/* channels on mKORG:
   44=cutoff
   71=resonance
   50=amplevel
   23=attack
   25=sustain
   26=release
*/

int Score::allocated_channels[] = {0, 0, 0, 0, 0, 0, 0}; // channels to send audio from tsunami to

// hard-coded list of BPMs of tracks stored on Tsunami's SD card.
// TODO: somehow make BPM accessible from file title
float Score::track_bpm[256] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
        1, 1, 90, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 73, 1, 1, 1,
        100, 1, 1, 1, 200, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 41, 1, 1,
        103, 1, 1, 1, 1, 1, 1, 93, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 78, 1, 100, 100, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 100, 1, 1, 1, 1,
        1, 1, 1, 1, 100, 60};