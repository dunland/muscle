#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
/*
    DD-200 test code compendium
*/

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_control_dd200(midi::MidiInterface<HardwareSerial> MIDI)
{
    // static float delay_time = 0;
    // static float delay_depth = 0;
    // static float delay_level = 0;

    switch (step)
    {

    case 0: // some ramp effect on snare. Used as intro for Concert at Stasi's. Works fine with DD200-DUAL mode @ ~150 BPM quarter notes

        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            Drumset::hihat->set_effect(TapTempo);
            Drumset::snare->setup_midi(dd200_DelayTime, Synthesizers::dd200, 89, 0, -9.96, 0.08);
            Drumset::snare->set_effect(Change_CC);

            setup = false;
        }

        /* CC-values are printed automatically */

        Hardware::lcd->setCursor(4, 1);
        Hardware::lcd->print("DUAL snaRamp");

        break;

        /* old code:
        case 1: // just using midi clock
            if (setup)
            {
                Drumset::hihat->set_effect(TapTempo);
                Drumset::snare->set_effect(Monitor);

                setup = false;

            }
            Hardware::lcd->setCursor(0, 0);
            Hardware::lcd->print(Globals::current_BPM);
            Hardware::lcd->setCursor(3, 0);
            Hardware::lcd->print("BPM");
            Hardware::lcd->setCursor(9, 1);
            Hardware::lcd->print("clock");

            break;

        case 10: // crash triggers dd200-time-reallocation
            if (setup)
            {
                delay_time = 0;
                setup = false;
                Drumset::hihat->set_effect(Monitor); // tap tempo would conflict with delay_time!
            }

            if (Drumset::crash1->timing.wasHit)
            {
                static int previous_tapInterval = 120;
                if (Globals::tapInterval != previous_tapInterval)
                {
                    int smallest_mod = 99999;
                    int idx = 0;
                    for (int i = 0; i < 128; i++) // starting at <2 will always result in mod = 0
                    {
                        int a = (Globals::tapInterval > Hardware::dd_200_midi_interval_map[i]) ? Globals::tapInterval : Hardware::dd_200_midi_interval_map[i];
                        int b = (Globals::tapInterval > Hardware::dd_200_midi_interval_map[i]) ? Hardware::dd_200_midi_interval_map[i] : Globals::tapInterval;
                        if (a % b <= smallest_mod) // smaller → update
                        {
                            if (random(2) > 0)
                            {
                                smallest_mod = a % b;
                                idx = i;
                            }
                        }
                        // else if (Globals::tapInterval % Hardware::dd_200_midi_interval_map[i] == smallest_mod) // equal → any of the two
                        // {
                        //     if (random(0, 2) > 0)
                        //     {
                        //         smallest_mod = Globals::tapInterval % Hardware::dd_200_midi_interval_map[i];
                        //         idx = i;
                        //     }
                        // }
                    }
                    delay_time = Hardware::dd_200_midi_interval_map[idx];
                    Synthesizers::dd200->sendControlChange(dd200_DelayTime, delay_time, MIDI);
                    previous_tapInterval = Globals::tapInterval;
                }
            }
            Hardware::lcd->setCursor(0, 0);
            Hardware::lcd->print(Globals::current_BPM);
            Hardware::lcd->setCursor(4, 0);
            Hardware::lcd->print(Globals::tapInterval);
            Hardware::lcd->setCursor(8, 0);
            Hardware::lcd->print(delay_time);
            Hardware::lcd->setCursor(8, 1);
            Hardware::lcd->print("realloc");

            break;

        case 3: // snare and kick increase delay_time, automatic decrease
            if (setup)
            {
                // notes.push_back(31);                              // G
                Synthesizers::mKorg->sendProgramChange(91, MIDI); // switches to Voice B.44
                Globals::bSendMidiClock = false;
                // Drumset::hihat->set_effect(TapTempo);
                setup = false;
            }

            // standard run: fall back to 0 quickly
            //   → instruments rise up
            static int previous_delay_time = 0;

            if (Drumset::snare->timing.wasHit || Drumset::kick->timing.wasHit)
            {
                delay_time += 5;
                if (delay_time > 127)
                    delay_time = 127;
            }

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

            Hardware::lcd->setCursor(9, 1);
            Hardware::lcd->print("KSramp");

            break;

        case 4:

            // snare:  delay_depth++; delay_level--
            // kick:   delay_depth--; delay_level++;


            if (setup)
            {
                Synthesizers::dd200->sendControlChange(dd200_DelayTime, 50, MIDI);
                setup = false;
            }

            if (Drumset::snare->timing.wasHit)
            {
                delay_depth += 1;
                if (delay_depth > 127)
                    delay_depth = 127;
                delay_level -= 1;
                if (delay_level < 0)
                    delay_level = 0;
            }
            if (Drumset::kick->timing.wasHit)
            {
                delay_level += 1;
                if (delay_level > 127)
                    delay_level = 127;
                delay_depth -= 1;
                if (delay_depth < 0)
                    delay_depth = 0;
            }

            Synthesizers::dd200->sendControlChange(dd200_DelayDepth, int(delay_depth), MIDI);
            Synthesizers::dd200->sendControlChange(dd200_DelayLevel, int(delay_level), MIDI);

            Hardware::lcd->setCursor(0, 0);
            Hardware::lcd->print(Globals::current_BPM);

            Hardware::lcd->setCursor(4, 0);
            Hardware::lcd->print(int(delay_depth));
            Hardware::lcd->setCursor(8, 0);
            Hardware::lcd->print(int(delay_level));

            break;

        case 6:

             // delay time affected by kick (strong) and snare (little);
             // effect level affected by kick and snare;
             // effect level decreasing automatically;
             // feedback is at about 80


            if (setup)
            {
                Hardware::footswitch_mode = No_Footswitch_Mode;
                Drumset::hihat->set_effect(TapTempo);
                Synthesizers::dd200->sendControlChange(dd200_DelayLevel, 80, MIDI); // I think 80 was a good value
                setup = false;
            }

            static bool bUseEffects = true;
            if (bUseEffects)
            {
                if (Drumset::kick->timing.wasHit)
                {
                    delay_time = min(127, delay_time + 5);
                    delay_level = min(127, delay_level + 1);
                }
                if (Drumset::snare->timing.wasHit) // snare has lower max vals
                {
                    delay_time = min(127, delay_time + 2);
                    delay_level = min(127, delay_level + 1);
                }

                delay_time = max(0, delay_time - 0.5);
                delay_level = max(0, delay_level - 0.3);
            }

            Synthesizers::dd200->sendControlChange(dd200_DelayDepth, int(delay_time), MIDI);
            Synthesizers::dd200->sendControlChange(dd200_DelayLevel, int(delay_level), MIDI);

            Hardware::lcd->setCursor(0, 0);
            Hardware::lcd->print(Globals::current_BPM);

            if (!bUseEffects)
            {
                Hardware::lcd->setCursor(3, 0);
                Hardware::lcd->print("/");
            }

            Hardware::lcd->setCursor(4, 0);
            Hardware::lcd->print(int(delay_time));
            Hardware::lcd->setCursor(8, 0);
            Hardware::lcd->print(int(delay_level));
            Hardware::lcd->setCursor(8, 0);
            Hardware::lcd->print("KSramp2");

            static float temp_delay_level;
            static uint64_t lastToggle = 0;

            if (Hardware::footswitch_is_pressed && millis() > lastToggle + 50) // tempo-linked stutter if on hold
            {
                if (bUseEffects) // save to temp
                {
                    temp_delay_level = delay_level;
                    delay_level = 0;
                }
                else // restore from temp
                {
                    delay_level = temp_delay_level;
                }
                bUseEffects = !bUseEffects;
                lastToggle = millis();
            }

            break;

            //////////////////////////////// TESTS ////////////////////////
            case 5: // test code! increases the delay time continuously.
                static unsigned long lastUpCount = millis();
                if (millis() > lastUpCount + 500)
                {
                    static int val = 0;
                    val = (val + 1) % 127;
                    Synthesizers::dd200->sendControlChange(dd200_DelayTime, val, MIDI);
                    lastUpCount = millis();

                    // print val
                    Hardware::lcd->setCursor(4, 0);
                    Hardware::lcd->print(val);
                    Hardware::lcd->setCursor(9, 1);
                    Hardware::lcd->print("testRise");
                }
                break;

            // case 5: // test code! increases the delay time when snare is hit, decrease via kick
            //     if (setup)
            //     {
            //         delay_time = 0;
            //         setup = false;
            //     }
            //     if (Drumset::kick->timing.wasHit)
            //     {
            //         delay_time -= 1;
            //         if (delay_time < 0)
            //             delay_time = 0;
            //     }

            //     if (Drumset::snare->timing.wasHit)
            //     {
            //         delay_time += 1;
            //         if (delay_time > 127)
            //             delay_time = 127;
            //     }

            //     Synthesizers::dd200->sendControlChange(dd200_DelayTime, delay_time, MIDI);

            //     // print val
            //     Hardware::lcd->setCursor(0, 0);
            //     Hardware::lcd->print(delay_time);

            //     break;
            */

    default:
        Synthesizers::mKorg->sendNoteOff(31, MIDI);
        proceed_to_next_score();
        step = 0;
        break;
    }
}