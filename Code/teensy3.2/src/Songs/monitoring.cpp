#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

///////////////////////////// MONITORING //////////////////////////////
void run_monitoring(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Hardware::footswitch_mode = Increment_Score;
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            // turn off all currently playing MIDI notes:
            for (int channel = 1; channel < 3; channel++)
            {
                for (int note_number = 0; note_number < 127; note_number++)
                {
                    MIDI.sendNoteOff(note_number, 127, channel);
                }
            }
        }
        
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(Globals::current_BPM);
        Hardware::lcd->setCursor(3, 0);
        Hardware::lcd->print("BPM");

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}