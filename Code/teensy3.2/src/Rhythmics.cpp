#include <Rhythmics.h>
#include <Globals.h>
#include <Serial.h>
#include <Instruments.h>
#include <Song.h>


void Rhythmics::run_beat(int last_beat_pos, std::vector<Instrument *> instruments)
{
    static boolean toggleLED = true;

    if (Globals::current_beat_pos != last_beat_pos)
    {
        // tidy up with previous beat position ----------------------------
        // apply topography derivations from previous beats
        // → problem: if there was any stroke at all, it was probably not on the very first run BEFORE derivation was executed

        if (Devtools::do_print_JSON)
            JSON::compose_and_send_json(instruments);

        // -------------------------- 32nd-notes --------------------------
        if (Devtools::do_send_to_processing)
        Devtools::println_to_console("");

        // print millis and current beat:
        if (Devtools::do_send_to_processing)
            Devtools::print_to_console("m");
        Devtools::print_to_console(String(millis()));
        Devtools::print_to_console("\t");
        // Devtools::print_to_console(Globals::current_eighth_count + 1); // if you want to print 8th-steps only
        if (Devtools::do_send_to_processing)
            Devtools::print_to_console("b");
        Devtools::print_to_console(Globals::current_beat_pos);
        Devtools::print_to_console("\t");

        // -------------------------- full notes: -------------------------
        if (Globals::current_beat_pos == 0)
        {
        }

        // ------------------------- quarter notes: -----------------------
        if (Globals::current_beat_pos % 8 == 0) // Globals::current_beat_pos holds 32 → %8 makes 4.
        {
        }

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

        for (auto &instrument : instruments)
        {
            Devtools::print_to_console(instrument->output_string);
            instrument->output_string = "\t";
        }
            Devtools::println_to_console("");

        // sum up all topographies of all instruments:
        // Globals::active_score->beat_sum.reset();
        // for (auto &instrument : instruments)
        // {
        //     if (instrument->drumtype != Ride && instrument->drumtype != Crash1 && instrument->drumtype != Crash2) // cymbals have too many counts
        //         Globals::active_score->beat_sum.add(&instrument->topography);
        // }
        // Globals::active_score->beat_sum.smoothen_dataArray();

        // Devtools::print_to_console("avg: ");
        // Devtools::print_to_console(Globals::active_score->beat_sum.average_smooth);
        // Devtools::print_to_console("/");
        // Devtools::print_to_console(Globals::active_score->beat_sum.activation_thresh);
        // Devtools::print_to_console("\t");
        // Devtools::print_to_console(Globals::active_score->name);
        // Devtools::print_to_console(".");
        // Devtools::println_to_console(Globals::active_score->step);

        // Globals::active_score->beat_regularity.derive_from(&Globals::active_score->beat_sum); // TODO: also do this for all instruments

        // print topo arrays:
        // if (Globals::do_print_beat_sum)
        // {
        //     // for (auto &instrument : instruments)
        //     // Devtools::printTopoArray(&instrument->topography);
        //     Devtools::printTopoArray(&Globals::active_score->beat_sum); // print volume layer
        // }
        // Devtools::printTopoArray(&active_score->beat_regularity);

        // perform timed pin actions according to current beat:
        for (auto &instrument : instruments)
        {
            instrument->perform(instruments);
        }
    }
}