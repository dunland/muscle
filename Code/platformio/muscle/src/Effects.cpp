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

void Effect::toggleRhythmSlot(Instrument *instrument)
{
    if (Globals::printStrokes)
        Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
    instrument->score.read_rhythm_slot[Globals::current_eighth_count] = !instrument->score.read_rhythm_slot[Globals::current_eighth_count];
}

void Effect::footswitch_recordSlots(Instrument *instrument) // record what is being played and replay it later
{
    if (Globals::printStrokes)
        Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
    instrument->score.set_rhythm_slot[Globals::current_eighth_count] = true;
}
