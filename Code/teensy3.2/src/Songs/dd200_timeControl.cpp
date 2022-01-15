#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void Score::run_dd200_timeControl(midi::MidiInterface<HardwareSerial> MIDI)
{
static int delay_time;


switch(step)
{
case 0:
   if (setup)
        {
            delay_time = 0;
            setup = false;
            Drumset::hihat->set_effect(TapTempo);
        }

        // detect BPM change:
        static int previous_tapInterval = 120;
        if (Globals::tapInterval != previous_tapInterval)
        {
            int smallest_mod = 99999;
            int idx = 0;
            for (int i = 2; i < 128; i++) // starting at <2 will always result in mod = 0
            {
                if (Globals::tapInterval % Hardware::dd_200_midi_time_map[i] < smallest_mod) // smaller → update
                {
                    smallest_mod = Globals::tapInterval % Hardware::dd_200_midi_time_map[i];
                    idx = i;
                }
                else if (Globals::tapInterval % Hardware::dd_200_midi_time_map[i] == smallest_mod) // equal → any of the two
                {
                    if (random(0, 1) >= 0.5)
                    {
                        smallest_mod = Globals::tapInterval % Hardware::dd_200_midi_time_map[i];
                        idx = i;
                    }
                }
            }
            delay_time = Hardware::dd_200_midi_time_map[idx];
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, delay_time, MIDI);
            previous_tapInterval = Globals::tapInterval;
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(4, 0);
        Hardware::lcd->print(Globals::tapInterval);
        Hardware::lcd->setCursor(8, 0);
        Hardware::lcd->print(delay_time);
        break;
}
}