#include <Arduino.h>
#include <Globals.h>

Calibration *calibration;

// Globals::numInputs = 7;
uint8_t Globals::leds[] = {LED_BUILTIN, LED_BUILTIN, LED_BUILTIN, LED_BUILTIN};
uint8_t Globals::pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
uint8_t Globals::pinAction[] = {8, 4, 8, 8, 8, 8, 1, 0}; // array to be changed within code loop.
/*
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with MIDI playback and volume changet
*/
