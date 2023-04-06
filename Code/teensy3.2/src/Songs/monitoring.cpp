#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

///////////////////////////// MONITORING //////////////////////////////
void Song::run_monitoring(midi::MidiInterface<HardwareSerial> MIDI)
// void Song::run_monitoring()
{
    switch (Song::step)
    {
    case 0:
        // if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            // Song::resetInstruments();
            // Song::notes.clear();

            // turn off all currently playing MIDI notes:
            for (int channel = 1; channel < 3; channel++)
            {
                for (int note_number = 0; note_number < 127; note_number++)
                {
                    // MIDI.sendNoteOff(note_number, 127, channel);
                }
            }

            // Song::setup = false;
        }
        break;

    default:
        // Song::proceed_to_next_score();
        break;
    }
}