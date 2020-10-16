#include <Instruments.h>
// #include <Globals.h>
// #include <Effects.h>

void Instrument::trigger(Instrument *instrument)
{
    switch (effect)
    {
    case PlayMidi:
        // Effect::playMidi(instrument);
        break;

    case Monitor:

        break;

    case ToggleRhythmSlot:

        break;

    case FootSwitchLooper:

        break;

    case TapTempo:

        break;

    case Swell:

        break;

    case TsunamiLink:

        break;

    case CymbalSwell:

        break;
    case TopographyLog:

        break;

    default:
        break;
    }
}