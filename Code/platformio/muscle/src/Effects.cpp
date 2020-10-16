#include <Effects.h> // take care this is always on top!
// #include <Instruments.h> // do not include anything here that is also in .h

// void Effect::playMidi(Instrument *instrument)
// {
//     MIDI.sendNoteOn(instrument->score.active_note, 127, 2);
//     instrument->score.last_notePlayed = millis();
// }

// void Effect::monitor(Instrument *instrument) // just prints what is being played.
// {
//     if (Globals::printStrokes)
//     {
//         // Globals::setInstrumentPrintString(instrument->drumtype, instrument->effect);
//     }
// }