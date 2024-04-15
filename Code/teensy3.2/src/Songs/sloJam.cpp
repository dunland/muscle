#include <Song.h>

#include <Instruments.h>
#include <Hardware.h>
#include <Notes.h>
#include <settings.h>

///////////////////////////// MONITORING //////////////////////////////
void run_sloJam()
{

    static int valueXgoal, valueYgoal, valueX, valueY;
    static FootSwitch *footswitch2 = new FootSwitch(FOOTSWITCH2);

    switch (Globals::active_song->step)
    {
    case 0: // 65 BPM
        if (Globals::active_song->get_setup_state())
        {
            for (auto &instrument : Drumset::instruments)
            {
                for (auto &midiTarget : instrument->midiTargets)
                {
                    delete midiTarget;
                    midiTarget = nullptr;
                }
                instrument->midiTargets.clear();
            }
            Devtools::println_to_console("SloJam: All midiTargets deleted!");

            Globals::current_BPM = 65;
            Globals::tapInterval = 60000 / Globals::current_BPM;
            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128);

            // FootSwitch::mode = SloJamPlaySample;

            Globals::active_song->resetInstruments();
            Globals::active_song->notes.clear();

            Synthesizers::mKorg->sendProgramChange(13); // A.26

            Synthesizers::kaossPad3->sendProgramChange(80); // Granulizer2
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, 60);
            Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, 100);
            Synthesizers::kaossPad3->sendControlChange(KP3_FX_Depth, 100);
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);

            Synthesizers::dd200->sendProgramChange(1);
            Synthesizers::dd200->sendControlChange(dd200_OnOff, 0); // OFF

            // turn off all currently playing MIDI notes:
            for (int channel = 1; channel < 3; channel++)
            {
                for (auto &synth : Synthesizers::synths)
                    for (int note_number = 0; note_number < 127; note_number++)
                    {
                        synth->sendNoteOff(note_number);
                    }
            }
        }

        Hardware::lcd->setCursor(6, 1);
        Hardware::lcd->print("drmFX");

        break;

    case 1: // keyboardpart nach dem loop: benutze footswitch

        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 0);
            footswitch2->callbackPressed = sloJamFootswitchFunction;
        }
        footswitch2->poll();

        Hardware::lcd->setCursor(6, 1);
        Hardware::lcd->print("sampl");

        break;

    case 2: // 169 BPM fest für superLoop part

        if (Globals::active_song->get_setup_state())
        {

            Synthesizers::mKorg->sendNoteOff(Note_A5);
            Synthesizers::mKorg->sendNoteOff(Note_A6);

            Globals::current_BPM = 169;
            Globals::tapInterval = 60000 / Globals::current_BPM;
            Globals::masterClock.begin(Globals::masterClockTimer, Globals::tapInterval * 1000 * 4 / 128);
        }

        Hardware::lcd->setCursor(6, 1);
        Hardware::lcd->print("loop");

        break;

    case 3: // triolensupport: stehende Note
        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::mKorg->sendNoteOn(Note_A5);
            Synthesizers::mKorg->sendNoteOn(Note_A6);
        }

        Hardware::lcd->setCursor(6, 1);
        Hardware::lcd->print("loopTrio");

        break;

    case 4: // das finale nach dem loop

        if (Globals::active_song->get_setup_state())
        {
            Synthesizers::kaossPad3->sendControlChange(KP3_Hold, 1);
            Synthesizers::kaossPad3->sendControlChange(92, 127); // Touch Pad on

            Drumset::standtom->addMidiTarget(mKORG_LFO1_Rate, Synthesizers::mKorg, 127, 32, 15, -0.07);
            Drumset::standtom->set_effect(Change_CC);
        }

        if (Drumset::snare->timing.wasHit)
        {
            valueXgoal = int(random(128));
            valueYgoal = int(random(128));
        }

        footswitch2->poll();

        // successive approximation:
        valueX = (valueX < valueXgoal) ? valueX + (valueXgoal - valueX) / 5 : valueX - (valueX - valueXgoal) / 5;
        Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_x, valueX);
        valueY = (valueY < valueYgoal) ? valueY + (valueYgoal - valueY) / 5 : valueY - (valueY - valueYgoal) / 5;
        Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_y, valueY);

        Hardware::lcd->setCursor(6, 1);
        Hardware::lcd->print("finale");

        break;

    default:
        Synthesizers::mKorg->sendNoteOff(Note_A5);
        Synthesizers::mKorg->sendNoteOff(Note_A6);
        Synthesizers::kaossPad3->sendControlChange(KP3_touch_pad_on_off, 0); // Touch Pad off

        Globals::active_song->proceed_to_next_score();

        delete footswitch2;
        footswitch2 = nullptr;

        break;
    }
}

void sloJamFootswitchFunction()
{
    static unsigned long lastTriggerMoment = 1000; // some pre-delay to prevent initial misdetection
    // static int eighthNoteDuration = int(60000.0 / float(Globals::current_BPM) * 2); funzt nicht...
    static int siebenAchtelNoten = 2307; // 60000 / 65 * 2 * 5
    static bool playNote = false;

    if (digitalRead(FOOTSWITCH2) == LOW && millis() > lastTriggerMoment + 50)
    {
        // play sample:
        Synthesizers::kaossPad3->sendNoteOn(KP3_Sample_A);

        lastTriggerMoment = millis();
        playNote = true;
    }

    // spiel A 7/8 später:
    if (millis() > (lastTriggerMoment + (siebenAchtelNoten)) && playNote)
    {
        static Notes mKorgNote = Note_A5;
        // turn note off:
        Synthesizers::mKorg->sendNoteOff(mKorgNote);
        // change note:
        mKorgNote = (mKorgNote == Note_A5) ? Note_A6 : Note_A5;
        // turn note on:
        Synthesizers::mKorg->sendNoteOn(mKorgNote);

        playNote = false;
    }
}