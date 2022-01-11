#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Score::run_control_dd200(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            // notes.push_back(31);                              // G
            Synthesizers::mKorg->sendProgramChange(91, MIDI); // switches to Voice B.44

            Drumset::hihat->set_effect(TapTempo);
            setup = false;
        }
        // playSingleNote(Synthesizers::mKorg, MIDI);

        /* standard run: fall back to 0 quickly
           → instruments rise up */
        static float delay_time = 0;
        static int previous_delay_time = 0;

        if (Drumset::crash1->timing.wasHit || Drumset::ride->timing.wasHit)
        {
            delay_time += 1;
            if (delay_time > 127)
                delay_time = 127;
        }

        // if (Drumset::standtom->timing.wasHit)
        // {
        //     delay_time += 5;
        //     if (delay_time > 127)
        //         delay_time = 127;
        // }

        delay_time -= 0.5;
        if (delay_time < 1)
            delay_time = 1;

        // send the value to dd200:
        if (previous_delay_time != delay_time) // prevent hangup at 0
        {
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, int(delay_time), MIDI);
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(delay_time);

        // BPM:
        Hardware::lcd->setCursor(7, 0);
        Hardware::lcd->print(Globals::current_BPM);

        break;

    case 1:

        if (setup)
        {
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 50, MIDI);
            setup = false;
        }

        static float delay_depth = 0;
        static float delay_level = 0;
        if (Drumset::crash1->timing.wasHit)
        {
            delay_depth += 1;
            if (delay_depth > 127)
                delay_depth = 127;
            delay_level -= 1;
            if (delay_level < 0)
                delay_level = 0;
        }
        if (Drumset::ride->timing.wasHit)
        {
            delay_level += 1;
            if (delay_level > 127)
                delay_level = 127;
            delay_depth -= 1;
            if (delay_depth < 0)
                delay_depth = 0;
        }

        Synthesizers::dd200->sendControlChange(dd200_DelayDepth, int(delay_depth), MIDI);
        Synthesizers::dd200->sendControlChange(dd200_DelayLevel, int(delay_level), MIDI); // TODO: dd200_DelayLevel implementieren!

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(delay_depth);
        Hardware::lcd->setCursor(6, 0);
        Hardware::lcd->print(delay_level);

        break;

    case 2: // test code! just increases the delay time continuously.
        static unsigned long lastUpCount = millis();
        if (millis() > lastUpCount + 500)
        {
            static int val = 0;
            val = (val + 1) % 127;
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, val, MIDI);
            lastUpCount = millis();

            // print val
            Hardware::lcd->setCursor(0, 0);
            Hardware::lcd->print(val);
        }
        break;

    case 3: // test code! increases the delay time when snare is hit, decrease via kick
        if (Drumset::kick->timing.wasHit)
        {
            delay_time -= 1;
            if (delay_time < 0)
                delay_time = 0;
        }

        if (Drumset::snare->timing.wasHit)
        {
            delay_time += 1;
            if (delay_time > 127)
                delay_time = 127;
        }
        break;

    default:
        Synthesizers::mKorg->sendNoteOff(31, MIDI);
        proceed_to_next_score();
        break;
    }
}