#include <Score/Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Score::run_control_dd200(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (step)
    {
    case 0:
        if (setup)
        {
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            notes.push_back(31);                              // G
            Synthesizers::mKorg->sendProgramChange(91, MIDI); // switches to Voice B.44

            Drumset::crash1->setup_midi(dd200_DelayTime, Synthesizers::dd200, 127, 0, -10, 0.2); // change
            Drumset::crash1->set_effect(Change_CC);
            Drumset::ride->setup_midi(DelayDepth, Synthesizers::dd200, 127, 0, -3, 0.1); // change
            Drumset::ride->set_effect(Change_CC);

            setup = false;
        }
        playSingleNote(Synthesizers::mKorg, MIDI);

        break;

    default:
        Synthesizers::mKorg->sendNoteOff(31, MIDI);
        proceed_to_next_score();
        break;
    }
}