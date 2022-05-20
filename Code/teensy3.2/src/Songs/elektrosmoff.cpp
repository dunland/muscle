#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// ELEKTROSMOFF /////////////////////////////
void Score::run_elektrosmoff(midi::MidiInterface<HardwareSerial> MIDI)
{
    // THIS SONG IS COMPOSED FOR microKORG A.81
    switch (step)
    {
    case 0: // Vocoder not activated
        if (setup)
        {
            Synthesizers::mKorg->sendProgramChange(56, MIDI); // selects mKORG Voice A.57
            Score::resetInstruments();                        // reset all instruments to "Monitor" mode
            Drumset::snare->set_effect(Monitor);
            Hardware::footswitch_mode = Increment_Score;
            Synthesizers::mKorg->midi_values[DelayDepth] = 0;
            setup = false;
        }
        break;

    case 1: // Snare → Vocoder fade in
        if (setup)
        {
            Synthesizers::mKorg->sendControlChange(DelayDepth, 0, MIDI);
            Drumset::snare->midi_settings.active_note = 55;                               // G = 55
            Drumset::snare->setup_midi(Amplevel, Synthesizers::mKorg, 127, 0, 3, -0.002); // changes Gate in Vocoder-Mode
            Drumset::snare->set_effect(Change_CC);
            Drumset::snare->midi_settings.synth->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment
            setup = false;
        }

        if (Drumset::snare->midi_settings.synth->notes[55] == false)
            Drumset::snare->midi_settings.synth->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment

        // proceed:
        if (Drumset::crash1->timing.wasHit)
        {
            increase_step();
        }
        break;

    case 2: // Snare → increase delay depth
        if (setup)
        {
            Synthesizers::mKorg->sendControlChange(Amplevel, 127, MIDI);
            Drumset::snare->midi_settings.cc_val = 0;
            Drumset::snare->setup_midi(DelayDepth, Synthesizers::mKorg, 90, 0, 3, -0.002);
            setup = false;
        }
        break;

    case 3: // kill switch
        if (setup)
        {
            Drumset::snare->set_effect(Monitor);
            Synthesizers::mKorg->sendControlChange(Amplevel, 0, MIDI);
            Synthesizers::mKorg->sendControlChange(DelayDepth, 0, MIDI);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI);
            setup = false;
        }
        break;

    default:
        proceed_to_next_score();
        break;
    }

    // SCORE END -------------------------------
}