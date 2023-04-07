#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_dd200_timeControl(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int delay_time;

    switch(Globals::active_song->step)
    {
    case 0: // just using midi clock
        if (Globals::active_song->setup)
        {
            Globals::active_song->setup = false;
            Drumset::hihat->set_effect(TapTempo);
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);

        break;

    case 1: // crash triggers dd200-time-reallocation
        if (Globals::active_song->setup)
        {
            delay_time = 0;
            Globals::active_song->setup = false;
            Drumset::hihat->set_effect(TapTempo);
        }

        if (Drumset::crash1->timing.wasHit)
        {

            // detect BPM change:
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

        Serial.println(random(2));
        break;

    default:
        Synthesizers::mKorg->sendNoteOff(31, MIDI);
        Globals::active_song->proceed_to_next_score();
        break;
    }
}