#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

//////////////////////////// CONTROL DD200 /////////////////////////////
void Song::run_whammyMountains(midi::MidiInterface<HardwareSerial> MIDI)
{
    /*
    WHAMMY MIDI CHANNEL = 4
    "CLASSIC MODE" (switch up)
    */

    static Synthesizer *whammy = Synthesizers::whammy;
    static int val = 0;

    switch (step)
    {

    case 0: // continuous test mode
        if (get_setup_state())
        {
            /* ------- general initialization routine -------------- */
            Hardware::footswitch_mode = Increment_Score;
            resetInstruments();
            notes.clear();

            whammy->sendProgramChange(1, MIDI); // selects Whammy ↑2 OCT

            
        }

        /* ---------------- general loop here ---------------------- */

        // just count upwards, slowly:

        static unsigned long lastChannelUp = 0;
        static unsigned long lastValUp = 0;
        static int channel = 0;
        if (millis() > (lastChannelUp + 500))
        {
            channel = (channel + 1) % 21;
            whammy->sendProgramChange(channel, MIDI); // sequentially change program
            lastChannelUp = millis();
        }

        if (millis() > (lastValUp + 250))
        {
            val = (val + 1) % 128;
            whammy->sendControlChange(11, val, MIDI); // 11 is for Expression Pedal
            lastValUp = millis();
        }

        // print channel
        Hardware::lcd->setCursor(5, 0);
        Hardware::lcd->print(channel);

        // print val
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(val);

        break;

    case 1:

        /* CONTROLLING DIGITECH WHAMMY VIA DRUMS AT MIDI CHANNEL 4 // CLASSIC MODE (pitch)*/
        if (get_setup_state())
        {
            
        }

        if (Drumset::snare->timing.wasHit) // snare increases expression by 5
        {
            val = (val + 5) % 128;
            whammy->sendControlChange(11, val, MIDI);
        }

        if (Drumset::kick->timing.wasHit) // kick decreases expression by 1
        {
            val = (val > 0) ? (val - 1) % 128 : 0;
            whammy->sendControlChange(11, val, MIDI);
        }

        if (Drumset::crash1->timing.wasHit) // crash changes program up
        {
            channel = (channel + 1) % 21;
            whammy->sendProgramChange(channel, MIDI);
        }
        if (Drumset::standtom->timing.wasHit) // ride changes program down
        {
            channel = (channel > 0) ? (channel - 1) % 21 : 0;
            whammy->sendProgramChange(channel, MIDI);
        }

        // print channel
        Hardware::lcd->setCursor(5, 0);
        Hardware::lcd->print(channel);

        // print val
        Hardware::lcd->setCursor(0, 0);
        Hardware::lcd->print(val);

        Globals::print_to_console("whammy channel: ");
        Globals::println_to_console(channel);
        Globals::print_to_console("whammy value: ");
        Globals::print_to_console(val);

        break;

    default:
        increase_step();
        break;
    }
}