#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

///////////////////////////// MONITORING //////////////////////////////
void Song::run_monitoring(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();
            setup = false;

            // turn off all currently playing MIDI notes:
            for (int channel = 1; channel < 3; channel++)
            {
                for (int note_number = 0; note_number < 127; note_number++)
                {
                    MIDI.sendNoteOff(note_number, 127, channel);
                }
            }
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }
}