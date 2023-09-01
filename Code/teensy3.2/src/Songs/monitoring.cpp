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

        Synthesizers::kaossPad3->sendControlChange(92, 0, MIDI); // Touch Pad off - JUST BECAUSE ALHAMBRA IS BEFORE!// TODO: execute this when leaving the song with push button! at best, by using callback functions for song.proceed_to_next_score

            Hardware::footswitch_mode = Increment_Score;
            Globals::active_song->resetInstrumentsFX();
            Globals::active_song->notes.list.clear();

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