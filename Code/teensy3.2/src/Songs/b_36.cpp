#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void run_b_36()
{
    switch (Globals::active_song->step)
    {
    case 0: // snare+standtom --> change mKORG Filter Type
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(85);

            Drumset::snare->set_effect(Change_CC);
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 127); // arp on
        }

        // TODO: test this! will simultaneous hit be detected?
        if (Drumset::snare->timing.wasHit && Drumset::standtom->timing.wasHit)
        {
            static int which_type; // 0~3=24LPF,12LPF,12BPF,12HPF
            which_type = (which_type + 1) % 4;
            Synthesizers::mKorg->sendControlChange(mKORG_Filter_Type, which_type);
        }

        Hardware::lcd->setCursor(3,0);
        Hardware::lcd->print("Snare+ST");

        break;

    case 1: // PlayMidi
        if (Globals::active_song->get_setup_state())
        {
            Globals::active_song->resetInstruments(); // reset all instruments to "Monitor" mode
            Globals::active_song->notes = {Note_B5, Note_E5, Note_B6, Note_E6};
            Synthesizers::mKorg->sendProgramChange(85); // load b_36

            Drumset::kick->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::kick->midiTargets.back()->notes.push_back(Globals::active_song->notes[0]);
            Drumset::kick->midiTargets.back()->active_note = Drumset::kick->midiTargets.back()->notes[0];
            Drumset::kick->set_effect(PlayMidi);

            // TODO: hitting the snare kills supermuscle! fix!!
            Drumset::snare->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::snare->midiTargets.back()->notes.push_back(Globals::active_song->notes[1]);
            Drumset::snare->midiTargets.back()->active_note = Drumset::snare->midiTargets.back()->notes[1];
            Drumset::snare->set_effect(PlayMidi);

            Drumset::standtom->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::standtom->midiTargets.back()->notes.push_back(Globals::active_song->notes[2]);
            Drumset::standtom->midiTargets.back()->active_note = Drumset::standtom->midiTargets.back()->notes[2];
            Drumset::standtom->set_effect(PlayMidi);

            Drumset::tom1->addMidiTarget(CC_None, Synthesizers::mKorg, 127, 0, 10, -0.1);
            Drumset::tom1->midiTargets.back()->notes.push_back(Globals::active_song->notes[3]);
            Drumset::tom1->midiTargets.back()->active_note = Drumset::tom1->midiTargets.back()->notes[3];
            Drumset::tom1->set_effect(PlayMidi);
        }

        Hardware::lcd->setCursor(3,0);
        Hardware::lcd->print("PlayMidi");

        break;

    default:
        Globals::active_song->proceed_to_next_score();
        // TODO: Destroy last created midiTargets!
        break;
    }
}