#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>
#include <Tsunami.h>
#include <MIDI.h>
#include <Globals.h>

// ------------------------- Debug variables --------------------------
class Debug
{
public:
    // startup settings:
    boolean use_responsiveCalibration = false;
    boolean printStrokes = true;
    boolean printNormalizedValues_ = false;
    boolean do_print_to_console = true;
    boolean do_send_to_processing = false;
    String output_string[Globals::numInputs];
    
enum DrumType
{
  Snare,
  Hihat,
  Kick,
  Tom1,
  Tom2,
  Standtom1,
  Cowbell,
  Standtom2,
  Ride,
  Crash1,
  Crash2
};

    String DrumtypeToHumanreadable(DrumType type);

    // FUNCTIONS:
    void setInstrumentPrintString(int instr, int incoming_pinAction);
    void print_to_console(String message_to_print);
    void println_to_console(String message_to_print);
    void send_to_processing(int message_to_send);
    void printNormalizedValues(boolean printNorm_criterion);
};

#endif