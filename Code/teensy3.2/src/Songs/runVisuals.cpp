#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// A.72 /////////////////////////////
void Song::runVisuals(midi::MidiInterface<HardwareSerial> MIDI)
{
    // skips thorugh visual levels by pressing footswitch

    static float delay_time = 0;

    switch (step)
    {
    case 0: // nix
        if (setup_song())
        {
            resetInstruments();
            notes.clear();

            Hardware::footswitch_mode = Increment_Score;

            step = 1;
        }
        break;

    case 1: // Theodolit
        if (setup_song())
        {
            
            Serial.println("Theodolit");
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("Theodolit");

        break;

    case 2: // Improvisation
            /* crash and ride increase delay_time, automatic decrease */
        if (setup_song())
        {
            
            Serial.println("Improvisation");

            notes.push_back(31);                              // G
            Synthesizers::mKorg->sendProgramChange(91, MIDI); // switches to Voice B.44

            Drumset::hihat->set_effect(TapTempo);
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("Improvisation");

        /* standard run: fall back to 0 quickly
           → instruments rise up */
        static int previous_delay_time = 0;

        if (Drumset::crash1->timing.wasHit || Drumset::ride->timing.wasHit)
        {
            delay_time += 1;
            if (delay_time > 127)
                delay_time = 127;
        }

        delay_time -= 0.5;
        if (delay_time < 1)
            delay_time = 1;

        // send the value to dd200:
        if (previous_delay_time != delay_time) // prevent hangup at 0
        {
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, int(delay_time), MIDI);
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(delay_time);

        // BPM:
        Hardware::lcd->setCursor(7, 0);
        Hardware::lcd->print(Globals::current_BPM);

        break;

    case 3: // Sattelstein 1
            /* keine Noten */
        if (setup_song())
        {
            Synthesizers::mKorg->sendProgramChange(38, MIDI); // selects mKORG Voice A.57
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 3, MIDI);
            
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("Sattelstein");

        break;

    case 4: // Sattelstein 2
        /* play notes G2 and G3 */
        if (setup_song())
        {
            
            Serial.println("Sattelstein");
            Globals::current_BPM = 139;
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("Sattelstein");

        if (Synthesizers::mKorg->notes[55] == false)
            Synthesizers::mKorg->sendNoteOn(55, MIDI); // play note 55 (G) if it is not playing at the moment
        if (Synthesizers::mKorg->notes[43] == false)
            Synthesizers::mKorg->sendNoteOn(43, MIDI); // play note 43 (G) if it is not playing at the moment
        break;

    case 5: // KupferUndGold
        if (setup_song())
        {
            resetInstruments();
            notes.clear();
            Synthesizers::mKorg->sendNoteOff(55, MIDI); // play note 55 (G) if it is not playing at the moment
            Synthesizers::mKorg->sendNoteOff(43, MIDI); // play note 43 (G) if it is not playing at the moment

            Serial.println("KupferUndGold");
            Synthesizers::mKorg->sendProgramChange(38, MIDI); // selects mKORG Voice A.57 to stop notes
            Synthesizers::dd200->sendControlChange(dd200_DelayTime, 3, MIDI); // delay_time = 82!
            
        }
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print("KupferUndGold");

        break;

    default:
        step = 1;
        setup_state = true;
        break;
    }
}