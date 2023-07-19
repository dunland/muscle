#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_b_36(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (Globals::active_song->step)
    {
    case 0: // snare+standtom --> change mKORG Filter Type
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(85, MIDI);

            Drumset::snare->set_effect(Change_CC);
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127, MIDI); // arp on
        }

        // TODO: test this! will simultaneous hit be detected?
        if (Drumset::snare->timing.wasHit && Drumset::standtom->timing.wasHit)
        {
            static int which_type; // 0~3=24LPF,12LPF,12BPF,12HPF
            which_type = (which_type + 1) % 4;
            Synthesizers::mKorg->sendControlChange(mKORG_Filter_Type, which_type, MIDI);
        }

        Hardware::lcd->setCursor(3,0);
        Hardware::lcd->print("Snare+ST");

        break;

    case 1: // PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
            Globals::active_song->notes = {Note_B5, Note_E5, Note_B6, Note_E6};
            Synthesizers::mKorg->sendProgramChange(85, MIDI); // load b_36

            Drumset::kick->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::kick->midi.notes.push_back(Globals::active_song->notes[0]);
            Drumset::kick->midi.active_note = Drumset::kick->midi.notes[0];
            Drumset::kick->set_effect(PlayMidi);

            // TODO: hitting the snare kills supermuscle! fix!!
            Drumset::snare->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::snare->midi.notes.push_back(Globals::active_song->notes[1]);
            Drumset::snare->midi.active_note = Drumset::snare->midi.notes[1];
            Drumset::snare->set_effect(PlayMidi);

            Drumset::standtom->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::standtom->midi.notes.push_back(Globals::active_song->notes[2]);
            Drumset::standtom->midi.active_note = Drumset::standtom->midi.notes[2];
            Drumset::standtom->set_effect(PlayMidi);

            Drumset::tom1->setup_midi(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->midi.notes.push_back(Globals::active_song->notes[3]);
            Drumset::tom1->midi.active_note = Drumset::tom1->midi.notes[3];
            Drumset::tom1->set_effect(PlayMidi);
        }

        Hardware::lcd->setCursor(3,0);
        Hardware::lcd->print("PlayMidi");

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }
}