#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

void run_b_11(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int current_val = 0;
    static int gravitate = map(0, -24, 24, 0, 127);

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(64, MIDI); // b.11
            // Globals::active_song->playSingleNote(Synthesizers::mKorg, MIDI);
        }

        static int randomNote = int(random(16,72));
        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[randomNote] == false)
            Synthesizers::mKorg->sendNoteOn(randomNote, MIDI);

        if (Drumset::snare->timing.wasHit)
            current_val -= 10;

        if (Drumset::kick->timing.wasHit)
            current_val += 10;

        // gravitate towards val:
        current_val = (current_val < gravitate) ? current_val + (gravitate - current_val) / 5 : current_val - (current_val - gravitate) / 2;

        Synthesizers::mKorg->sendControlChange(mKORG_Osc2_semitone, current_val, MIDI);

        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(current_val);

        break;

    default:
        Globals::active_song->setup_state = true;
        Globals::active_song->step = 0;
        break;
    }
}