#include <Arduino.h>
#include <Globals.h>
#include <Debug.h>
#include <Core.h>
#include <Calibration.h>

// ------------------ set string for play logging: --------------------
void Debug::setInstrumentPrintString(int instr, int incoming_pinAction)
{
    switch (incoming_pinAction)
    {

    case 1: // monitor: just print what is being played
        if (instr == Kick)
            output_string[instr] = "■\t"; // Kickdrum
        else if (instr == Cowbell)
            output_string[instr] = "▲\t"; // Crash
        else if (instr == Standtom1)
            output_string[instr] = "□\t"; // Standtom
        else if (instr == Standtom2)
            output_string[instr] = "O\t"; // Standtom
        else if (instr == Hihat)
            output_string[instr] = "x\t"; // Hi-Hat
        else if (instr == Tom1)
            output_string[instr] = "°\t"; // Tom 1
        else if (instr == Snare)
            output_string[instr] = "※\t"; // Snaredrum
        else if (instr == Tom2)
            output_string[instr] = "o\t"; // Tom 2
        else if (instr == Ride && Globals::numInputs > 7)
            output_string[instr] = "!xx\t"; // Ride
        else if (instr == Crash1 && Globals::numInputs > 7)
            output_string[instr] = "!-X-\t"; // Crash
        else if (instr == Crash2 && Globals::numInputs > 7)
            output_string[instr] = "!-XX-\t"; // Crash
        break;

    case 2: // toggle beat slot
        if (instr == Kick)
            output_string[instr] = "■\t"; // Kickdrum
        else if (instr == Cowbell)
            output_string[instr] = "▲\t"; // Crash
        else if (instr == Standtom1)
            output_string[instr] = "□\t"; // Standtom
        else if (instr == Standtom2)
            output_string[instr] = "O\t"; // Standtom
        else if (instr == Hihat)
            output_string[instr] = "x\t"; // Hi-Hat
        else if (instr == Tom1)
            output_string[instr] = "°\t"; // Tom 1
        else if (instr == Snare)
            output_string[instr] = "※\t"; // Snaredrum
        else if (instr == Tom2)
            output_string[instr] = "o\t"; // Tom 2
        else if (instr == Ride && Globals::numInputs > 7)
            output_string[instr] = "!xx\t"; // Ride
        else if (instr == Crash1 && Globals::numInputs > 7)
            output_string[instr] = "!-X-\t"; // Crash
        else if (instr == Crash2 && Globals::numInputs > 7)
            output_string[instr] = "!-XX-\t"; // Crash
        break;

    case 3: // add an ! if pinAction == 3 (replay logged rhythm)
        if (instr == Kick)
            output_string[instr] = "!■\t"; // Kickdrum
        else if (instr == Cowbell)
            output_string[instr] = "!▲\t"; // Crash
        else if (instr == Standtom1)
            output_string[instr] = "!□\t"; // Standtom
        else if (instr == Standtom2)
            output_string[instr] = "!O\t"; // Standtom
        else if (instr == Hihat)
            output_string[instr] = "!x\t"; // Hi-Hat
        else if (instr == Tom1)
            output_string[instr] = "!°\t"; // Tom 1
        else if (instr == Snare)
            output_string[instr] = "!※\t"; // Snaredrum
        else if (instr == Tom2)
            output_string[instr] = "!o\t"; // Tom 2
        else if (instr == Ride && Globals::numInputs > 7)
            output_string[instr] = "!xx\t"; // Ride
        else if (instr == Crash1 && Globals::numInputs > 7)
            output_string[instr] = "!-X-\t"; // Crash
        else if (instr == Crash2 && Globals::numInputs > 7)
            output_string[instr] = "!-XX-\t"; // Crash
        break;

        // case 5: // print swell_val for repeated MIDI notes in "swell" mode
        //   output_string[instr] = swell_val[instr];
        //   output_string[instr] += "\t";
        //   break;
    }
}

// ------------------------ print logs --------------------------------
// print the play log to Arduino console:
void Debug::print_to_console(String message_to_print)
{
    if (do_print_to_console)
        Serial.print(message_to_print);
}

void Debug::println_to_console(String message_to_print)
{
    if (do_print_to_console)
        Serial.println(message_to_print);
}

// or send stuff to processing:
void Debug::send_to_processing(int message_to_send)
{
    if (do_send_to_processing)
        Serial.write(message_to_send);
}

// print normalized values:
void Debug::printNormalizedValues(boolean printNorm_criterion)
{
    // useful debugger for column-wise output of raw/normalised values:

    if (printNorm_criterion == true)
    {
        static unsigned long lastMillis;
        if (millis() != lastMillis)
        {
            for (int i = 0; i < Globals::numInputs; i++)
            {
                // static int countsCopy[Globals::numInputs];
                //noInterrupts();
                //countsCopy[i] = counts[i];
                //interrupts();
                //Serial.print(Globals::pins[i]);
                //Serial.print(":\t");
                Serial.print(Core::pinValue(i, Calibration::noiseFloor));
                Serial.print("\t");
                //Serial.print(", ");
                //Serial.print(countsCopy[i]);
            }
            Serial.println("");
        }
        lastMillis = millis();
    }
}

String Debug::DrumtypeToHumanreadable(DrumType type)
{
  switch (type)
  {
  case Snare:
    return "Snare";
  case Hihat:
    return "Hihat";
  case Kick:
    return "Kick";
  case Tom1:
    return "Tom1";
  case Tom2:
    return "Tom2";
  case Standtom1:
    return "Standtom1";
  case Standtom2:
    return "Standtom2";
  case Ride:
    return "Ride";
  case Crash1:
    return "Crash1";
  case Crash2:
    return "Crash2";
  case Cowbell:
    return "Cowbell";
  }
  return "";
}