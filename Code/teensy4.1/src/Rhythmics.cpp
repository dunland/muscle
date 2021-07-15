#include <Rhythmics.h>
#include <Globals.h>
#include <JSON.h>
#include <Instruments.h>
#include <Score/Score.h>
#include <MIDI.h>

void Rhythmics::run_beat(int last_beat_pos, std::vector<Instrument *> instruments, midi::MidiInterface<HardwareSerial> MIDI)
{
    static boolean toggleLED = true;

    if (Globals::current_beat_pos != last_beat_pos)
    {
        // tidy up with previous beat position ----------------------------
        // apply topography derivations from previous beats
        // → problem: if there was any stroke at all, it was probably not on the very first run BEFORE derivation was executed

        if (Globals::do_print_JSON)
            JSON::compose_and_send_json(instruments);

        // -------------------------- 32nd-notes --------------------------
        Serial.println("");
        
        // print millis and current beat:
        if (Globals::do_send_to_processing)
            Globals::print_to_console("m");
        Globals::print_to_console(String(millis()));
        Globals::print_to_console("\t");
        // Globals::print_to_console(Globals::current_eighth_count + 1); // if you want to print 8th-steps only
        if (Globals::do_send_to_processing)
            Globals::print_to_console("b");
        Globals::print_to_console(Globals::current_beat_pos);
        Globals::print_to_console("\t");

        // -------------------------- full notes: -------------------------
        if (Globals::current_beat_pos == 0)
        {
            // testing visuals:
            // Serial.print("hit");
            // Serial.println("Snare");
        }

        // ------------------------- quarter notes: -----------------------
        if (Globals::current_beat_pos % 8 == 0) // Globals::current_beat_pos holds 32 → %8 makes 4.
        {
            // Hardware::vibrate_motor(50);
            // mKorg->sendNoteOn(50, MIDI);
            // MIDI.sendNoteOn(50, 127, 2);
        }
        // Debug: play MIDI note on quarter notes
        // if (Globals::current_beat_pos % 8 == 0)
        // {
        //   MIDI.sendNoteOn(57, 127, 2);
        // }
        // else
        // {
        //   MIDI.sendNoteOff(57, 127, 2);
        // }

        // --------------------------- 8th notes: -------------------------
        if (Globals::current_beat_pos % 4 == 0)
        {
            // increase 8th note counter:
            Globals::current_eighth_count = (Globals::current_eighth_count + 1) % 8;
            toggleLED = !toggleLED;

            // blink LED rhythmically:
            digitalWrite(LED_BUILTIN, toggleLED);
        }

        // --------------------------- 16th notes: ------------------------
        if (Globals::current_beat_pos % 2 == 0)
        {
            // increase 16th note counter:
            Globals::current_16th_count = (Globals::current_16th_count + 1) % 16;
        }

        // ----------------------------- draw play log to console

        // Globals::print_to_console(Globals::current_beat_pos / 4);
        // Globals::print_to_console("\t");
        // Globals::print_to_console(Globals::current_eighth_count);
        for (auto &instrument : instruments)
        {
            Globals::print_to_console(instrument->output_string);
            instrument->output_string = "\t";
        }

        // sum up all topographies of all instruments:
        Globals::active_score->beat_sum.reset();
        for (auto &instrument : instruments)
        {
            if (instrument->drumtype != Ride && instrument->drumtype != Crash1 && instrument->drumtype != Crash2) // cymbals have too many counts
                Globals::active_score->beat_sum.add(&instrument->topography);
        }
        Globals::active_score->beat_sum.smoothen_dataArray();

        Globals::print_to_console("avg: ");
        Globals::print_to_console(Globals::active_score->beat_sum.average_smooth);
        Globals::print_to_console("/");
        Globals::print_to_console(Globals::active_score->beat_sum.activation_thresh);
        Globals::print_to_console("\t");
        Globals::print_to_console(Globals::active_score->name);
        Globals::print_to_console(".");
        Globals::println_to_console(Globals::active_score->step);

        Globals::active_score->beat_regularity.derive_from(&Globals::active_score->beat_sum); // TODO: also do this for all instruments

        // print topo arrays:
        if (Globals::do_print_beat_sum)
        {
            // for (auto &instrument : instruments)
            // Globals::printTopoArray(&instrument->topography);
            Globals::printTopoArray(&Globals::active_score->beat_sum); // print volume layer
        }
        // Globals::printTopoArray(&active_score->beat_regularity);

        // perform timed pin actions according to current beat:
        for (auto &instrument : instruments)
        {
            instrument->perform(instruments, MIDI);
        }
    }
}