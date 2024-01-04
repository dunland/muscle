#pragma once
#include <Arduino.h>

class Devtools
{

public:
    // ------------------------- Debug variables --------------------------
    static boolean do_print_to_console;
    static boolean do_send_to_processing;
    static boolean printStrokes;
    static boolean use_responsiveCalibration;
    static boolean use_serial_comm;
    static boolean do_print_beat_sum; // prints Score::beat_sum topography array
    static boolean do_print_JSON;     // determines whether to use USB Serial communication for monitoring via processing/console or not
    static bool overwrite_SD_data;

    // DEBUG FUNCTIONS: ---------------------------------------------------
    // print the play log to Serial monitor:
    static void print_to_console(String message_to_print);
    static void print_to_console(int int_to_print);
    static void print_to_console(float float_to_print);

    static void println_to_console(String message_to_print);
    static void println_to_console(int int_to_print);
    static void println_to_console(float float_to_print);
    // --------------------------------------------------------------------
};