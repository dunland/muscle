#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void run_A_72(midi::MidiInterface<HardwareSerial> MIDI)
{
    // each step:
    // - switch between Timbre 1 and 2
    // - add a note to Score::notes
    // - increase note-index
    // → play latest 4 notes

    int note_increase = 4; // notes are in turns added by 4 or 5

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();
            Globals::active_song->notes.push_back(int(random(12, 24)));

            Hardware::footswitch_mode = Increment_Score;
            Synthesizers::mKorg->sendProgramChange(49, MIDI); // switches to Voice A.72
            delay(200);
            Synthesizers::mKorg->sendControlChange(mKORG_TimbreSelect, 1, MIDI); // Select Timbre 2

            Drumset::tom1->set_effect(Change_CC);
            Drumset::tom1->setup_midi(mKORG_Cutoff, Synthesizers::mKorg, 127, 29, 3, -0.035);

            Drumset::standtom->set_effect(Change_CC);
            Drumset::standtom->setup_midi(mKORG_Resonance, Synthesizers::mKorg, 127, 29, 5, -0.05);

            Drumset::kick->midi.notes.push_back(Globals::active_song->notes[0] + 44);
            Drumset::kick->midi.active_note = Drumset::kick->midi.notes[0];
            Drumset::kick->set_effect(PlayMidi);

            Drumset::snare->midi.notes.push_back(Globals::active_song->notes[0] + 49);
            Drumset::snare->midi.active_note = Drumset::snare->midi.notes[0];
            Drumset::snare->set_effect(PlayMidi);

        }
        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] == false)
            Synthesizers::mKorg->sendNoteOn(Globals::active_song->notes[Globals::active_song->note_idx], MIDI);

        // increasing amplitude until max:
        static float val = 0;
        val += 0.5;
        Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, int(val), MIDI);

        Hardware::lcd->setCursor(10, 0);
        Hardware::lcd->print(val);

        if (Synthesizers::mKorg->midi_values[mKORG_Amplevel] >= 126)
            Globals::active_song->step = 1;
        break;

    case 1:
        if (Globals::active_song->get_setup_state())
        {
            int val = (Synthesizers::mKorg->midi_values[mKORG_TimbreSelect] == 0) ? 127 : 0;
            Synthesizers::mKorg->sendControlChange(mKORG_TimbreSelect, val, MIDI); // Select Timbre 2
        }

        if (Globals::current_beat_pos == 0 && Synthesizers::mKorg->notes[Globals::active_song->notes[Globals::active_song->note_idx]] == false)
            Synthesizers::mKorg->sendNoteOn(Globals::active_song->notes[Globals::active_song->note_idx], MIDI);

        break;

    default:
        Globals::active_song->step = 1;
        Globals::active_song->setup_state = true;
        note_increase = (note_increase == 4) ? 5 : 4;
        Globals::active_song->notes.push_back(Globals::active_song->notes[Globals::active_song->note_idx] + note_increase);
        Globals::active_song->note_idx++;
        break;
    }
}