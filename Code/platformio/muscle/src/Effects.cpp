#include <Effects.h> // take care this is always on top!
// #include <Instruments.h> // do not include anything here that is also in .h
#include <Globals.h>
#include <MIDI.h>

///////////////////////////// TRIGGER EFFECTS /////////////////////////
///////////////////////////////////////////////////////////////////////

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

void Effect::getTapTempo()
{
    static unsigned long timeSinceFirstTap = 0;
    static int tapState = 1;
    static int num_of_taps = 0;
    static int clock_sum = 0;

    switch (tapState)
    {
        //    case 0: // this is for activation of tap tempo listen
        //      tapState = 1;
        //      break;

    case 1:                                       // first hit
        if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
        {
            num_of_taps = 0;
            clock_sum = 0;
            Serial.println("-----------TAP RESET!-----------\n");
        }
        timeSinceFirstTap = millis(); // record time of first hit
        tapState = 2;                 // next: wait for second hit

        break;

    case 2: // second hit

        if (millis() < timeSinceFirstTap + 2000) // only record tap if interval was not too long
        {
            num_of_taps++;
            clock_sum += millis() - timeSinceFirstTap;
            Globals::tapInterval = clock_sum / num_of_taps;
            Serial.print("new tap Tempo is ");
            Serial.print(60000 / Globals::tapInterval);
            Serial.print(" bpm (");
            Serial.print(Globals::tapInterval);
            Serial.println(" ms interval)");

            Globals::current_BPM = 60000 / Globals::tapInterval;
            tapState = 1;

            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        }

        if (timeSinceFirstTap > 2000) // forget tap if time was too long
        {
            tapState = 1;
            // Serial.println("too long...");
        }
        // }
        break;
    }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////
