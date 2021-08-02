#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// SATTELSTEIN /////////////////////////////
// THIS SONG IS COMPOSED FOR microKORG A.57
void Score::run_sattelstein(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Score::resetInstruments();                        // reset all instruments to "Monitor" mode
            Synthesizers::mKorg->sendProgramChange(38, MIDI); // selects mKORG Voice A.57
            Hardware::footswitch_mode = Increment_Score;
            setup = false;
        }
        break;

    case 1: // play notes G2 and G3
        if (setup)
        {
            setup = false;
        }

        if (Synthesizers::mKorg->notes[55] == false)
            Synthesizers::mKorg->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment
        if (Synthesizers::mKorg->notes[43] == false)
            Synthesizers::mKorg->sendNoteOn(43, MIDI); // play note 43 (G) if it is not playing at the moment
        break;

    case 2: // stop playing notes and leave
        if (setup)
        {
            // Synthesizers::mKorg->sendNoteOff(55, MIDI); // play note 55 (G) if it is not playing at the moment
            // Synthesizers::mKorg->sendNoteOff(43, MIDI); // play note 43 (G) if it is not playing at the moment
            // MIDI.sendRealTime(midi::Stop); // TODO: make this work!
            Synthesizers::mKorg->sendProgramChange(38, MIDI); // selects mKORG Voice A.57

            setup = false;
        }
        increase_step();
        break;

    default:

        proceed_to_next_score();
        break;
    }
}