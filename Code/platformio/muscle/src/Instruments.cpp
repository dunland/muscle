#include <Instruments.h>
#include <MIDI.h>
#include <Tsunami.h>

void Instrument::trigger(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (effect)
    {
    case PlayMidi:
        Effect::playMidi(instrument, MIDI);
        break;

    case Monitor:
        Effect::monitor(instrument);
        break;

    case ToggleRhythmSlot:
        Effect::toggleRhythmSlot(instrument);
        break;

    case FootSwitchLooper:
        Effect::footswitch_recordSlots(instrument);
        break;

    case TapTempo:
        Effect::getTapTempo();
        break;

    case Swell:
        Effect::swell_rec(instrument, MIDI);
        break;

    case TsunamiLink:

        break;

    case CymbalSwell: // swell-effect for loudness on field recordings (use on cymbals e.g.)
        // TODO: UNTESTED! (2020-10-09)
        Effect::swell_rec(instrument, MIDI);
        break;
    case TopographyLog:

        break;

    default:
        break;
    }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

void Instrument::perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (effect)
    {
    case PlayMidi:
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
        Effect::swell_perform(instrument, MIDI);
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