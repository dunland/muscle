#include <Song.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>

//////////////////////////// A.72 /////////////////////////////
void run_control_volca(midi::MidiInterface<HardwareSerial> MIDI)
{
    static int grundton = Note_D4;
    static uint64_t lastNoteSent = 0;
    Synthesizer *volca = Synthesizers::volca;

    switch (Globals::active_song->step)
    {
    case 0:
        if (Globals::active_song->setup)
        {
            Globals::active_song->setup = false;
        }

        if (Drumset::kick->timing.wasHit)
        {
            volca->sendNoteOn(grundton, MIDI);
            lastNoteSent = millis();
        }

        if (millis() > lastNoteSent + 50){
            volca->sendNoteOff(grundton, MIDI);
        }

        // Hardware::lcd->setCursor(10, 0);
        // Hardware::lcd->print();

        break;

    default:
    Globals::active_song->proceed_to_next_score();
        break;
    }
}