#include <Effects.h> // take care this is always on top!
// #include <Instruments.h> // do not include anything here that is also in .h
#include <Globals.h>
#include <MIDI.h>

// midi::MidiInterface<HardwareSerial> MIDI((HardwareSerial&)Serial2);

void Effect::playMidi(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    MIDI.sendNoteOn(instrument->score.active_note, 127, 2);
    instrument->score.last_notePlayed = millis();
}

void Effect::monitor(Instrument *instrument) // just prints what is being played.
{
    if (Globals::printStrokes)
    {
        Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
    }
}

// void Effect::toggleRhythmSlot(instrument)
// {
//     if (Globals::printStrokes)
//         Globals::setInstrumentPrintString(i, instruments[i]->effect);
//     read_rhythm_slot[i][current_eighth_count] = !read_rhythm_slot[i][current_eighth_count];
// }