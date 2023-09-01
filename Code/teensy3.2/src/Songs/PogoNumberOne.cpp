#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// A.72 /////////////////////////////
void run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->get_setup_state())
        {
             Globals::active_song->notes.list = {Note_D5, Note_E5, Note_A4, Note_D6, Note_E6};

            Globals::bSendMidiClock = false;
            Synthesizers::mKorg->sendControlChange(mKORG_Arpeggio_onOff, 0, MIDI); // Apreggiator on
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_on_off, 0, MIDI); // Apreggiator on
            Synthesizers::kaossPad3->sendControlChange(KP3_Sample_A, 0, MIDI); // Apreggiator on
            Synthesizers::kaossPad3->sendControlChange(KP3_Sample_B, 0, MIDI); // Apreggiator on
            Synthesizers::kaossPad3->sendControlChange(KP3_Sample_C, 0, MIDI); // Apreggiator on
            Synthesizers::kaossPad3->sendControlChange(KP3_Sample_D, 0, MIDI); // Apreggiator on

            Synthesizers::whammy->sendProgramChange(42, MIDI); // WHAMMY UP 2 OCT

            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 4, MIDI);
            Synthesizers::dd200->sendProgramChange(3, MIDI);  // Program #3

            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0, MIDI); // ATTENTION: must be after programChange!
            Hardware::footswitch_mode = Increment_Score;

        }
        break;

    default:
        Globals::bSendMidiClock = true;
        Globals::active_song->proceed_to_next_score();
        break;
    }
}