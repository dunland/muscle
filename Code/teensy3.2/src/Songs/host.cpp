#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// ELEKTROSMOFF /////////////////////////////
void run_host()
{
    // THIS SONG IS COMPOSED FOR microKORG A.81
    switch (Globals::active_song->step)
    {
    case 0: // Vocoder not activated
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendProgramChange(56); // selects mKORG Voice A.57
            Globals::active_song->resetInstruments();         // reset all instruments to "Monitor" mode
            Drumset::snare->set_effect(Monitor);
            Hardware::footswitch_mode = Increment_Score;
            Synthesizers::mKorg->midi_values[mKORG_DelayDepth] = 0;
        }
    case 1:

        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendControlChange(mKORG_DelayDepth, 0);
            Drumset::snare->addMidiTarget(mKORG_Amplevel, Synthesizers::mKorg, 127, 0, 3, -0.002); // changes Gate in Vocoder-Mode
            Drumset::snare->midiTargets.back()->active_note = 55;                                     // G = 55
            Drumset::snare->set_effect(Change_CC);
            Drumset::snare->midiTargets.back()->synth->sendNoteOn(55); // play note 55 (G) if it is not playing at the moment
        }

        if (Drumset::snare->midiTargets.back()->synth->notes[55] == false)
            Drumset::snare->midiTargets.back()->synth->sendNoteOn(55); // play note 55 (G) if it is not playing at the moment

        // proceed:
        if (Drumset::crash1->timing.wasHit)
        {
            Globals::active_song->increase_step();
        }
        break;

    case 2: // Snare → increase delay depth
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 127);
            Drumset::snare->midiTargets.back()->cc_val = 0;
            Drumset::snare->addMidiTarget(mKORG_DelayDepth, Synthesizers::mKorg, 90, 0, 3, -0.002);
        }
        break;

    case 3: // kill switch
        if (Globals::active_song->get_setup_state())
        {
            Drumset::snare->set_effect(Monitor);
            Synthesizers::mKorg->sendControlChange(mKORG_Amplevel, 0);
            Synthesizers::mKorg->sendControlChange(mKORG_DelayDepth, 0);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0);
        }
        break;

    default:
        Globals::active_song->proceed_to_next_score();
        break;
    }

    // SCORE END -------------------------------
}