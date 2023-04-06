#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// SATTELSTEIN /////////////////////////////
// THIS SONG IS COMPOSED FOR microKORG A.57
void run_sattelstein(midi::MidiInterface<HardwareSerial> MIDI)
{
    switch(Song::step)
    {
    case 0:
        if (Song::setup)
        {
            Globals::bSendMidiClock = true;
            Song::resetInstruments();                        // reset all instruments to "Monitor" mode
            Synthesizers::mKorg->sendProgramChange(38, MIDI); // selects mKORG Voice A.57
            Hardware::footswitch_mode = Increment_Score;
            Globals::tapInterval = 392; // 153 BPM
            Globals::current_BPM = 60000 / Globals::tapInterval;
            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
            Song::setup = false;
        }
        break;

    case 1: // play notes G2 and G3
        if (Song::setup)
        {
            Song::setup = false;
        }

        if (Synthesizers::mKorg->notes[55] == false)
            Synthesizers::mKorg->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment
        if (Synthesizers::mKorg->notes[43] == false)
            Synthesizers::mKorg->sendNoteOn(43, MIDI); // play note 43 (G) if it is not playing at the moment
        break;

    case 2: // stop playing notes and leave
        if (Song::setup)
        {
            Synthesizers::mKorg->sendNoteOff(55, MIDI); // play note 55 (G) if it is not playing at the moment
            Synthesizers::mKorg->sendNoteOff(43, MIDI); // play note 43 (G) if it is not playing at the moment
            // MIDI.sendRealTime(midi::Stop); // TODO: make this work!
            Synthesizers::mKorg->sendProgramChange(38, MIDI); // selects mKORG Voice A.57

            Song::setup = false;
        }
        Song::increase_step();
        break;

    default:

        Song::proceed_to_next_score();
        break;
    }
}