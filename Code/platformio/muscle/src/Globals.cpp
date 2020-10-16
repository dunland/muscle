#include <Globals.h>
// boolean Globals::printStrokes = true;
String Globals::output_string[Globals::numInputs];


///////////////////// SET STRING FOR PLAY LOGGING /////////////////////
///////////////////////////////////////////////////////////////////////

// int swell_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

void Globals::setInstrumentPrintString(DrumType drum, EffectsType effect)
{
    switch (effect)
    {

    case Monitor: // monitor: just print what is being played
        if (drum == Kick)
            Globals::output_string[drum] = "■\t"; // Kickdrum
        else if (drum == Cowbell)
            Globals::output_string[drum] = "▲\t"; // Crash
        else if (drum == Standtom1)
            Globals::output_string[drum] = "□\t"; // Standtom
        else if (drum == Standtom2)
            Globals::output_string[drum] = "O\t"; // Standtom
        else if (drum == Hihat)
            Globals::output_string[drum] = "x\t"; // Hi-Hat
        else if (drum == Tom1)
            Globals::output_string[drum] = "°\t"; // Tom 1
        else if (drum == Snare)
            Globals::output_string[drum] = "※\t"; // Snaredrum
        else if (drum == Tom2)
            Globals::output_string[drum] = "o\t"; // Tom 2
        else if (drum == Ride)
            Globals::output_string[drum] = "xx\t"; // Ride
        else if (drum == Crash1)
            Globals::output_string[drum] = "-X-\t"; // Crash
        else if (drum == Crash2)
            Globals::output_string[drum] = "-XX-\t"; // Crash
        break;

    case ToggleRhythmSlot: // toggle beat slot
        if (drum == Kick)
            Globals::output_string[drum] = "■\t"; // Kickdrum
        else if (drum == Cowbell)
            Globals::output_string[drum] = "▲\t"; // Crash
        else if (drum == Standtom1)
            Globals::output_string[drum] = "□\t"; // Standtom
        else if (drum == Standtom2)
            Globals::output_string[drum] = "O\t"; // Standtom
        else if (drum == Hihat)
            Globals::output_string[drum] = "x\t"; // Hi-Hat
        else if (drum == Tom1)
            Globals::output_string[drum] = "°\t"; // Tom 1
        else if (drum == Snare)
            Globals::output_string[drum] = "※\t"; // Snaredrum
        else if (drum == Tom2)
            Globals::output_string[drum] = "o\t"; // Tom 2
        else if (drum == Ride)
            Globals::output_string[drum] = "xx\t"; // Ride
        else if (drum == Crash1)
            Globals::output_string[drum] = "-X-\t"; // Crash
        else if (drum == Crash2)
            Globals::output_string[drum] = "-XX-\t"; // Crash
        break;

    case FootSwitchLooper: // add an ! if pinAction == 3 (replay logged rhythm)
        if (drum == Kick)
            Globals::output_string[drum] = "!■\t"; // Kickdrum
        else if (drum == Cowbell)
            Globals::output_string[drum] = "!▲\t"; // Crash
        else if (drum == Standtom1)
            Globals::output_string[drum] = "!□\t"; // Standtom
        else if (drum == Standtom2)
            Globals::output_string[drum] = "!O\t"; // Standtom
        else if (drum == Hihat)
            Globals::output_string[drum] = "!x\t"; // Hi-Hat
        else if (drum == Tom1)
            Globals::output_string[drum] = "!°\t"; // Tom 1
        else if (drum == Snare)
            Globals::output_string[drum] = "!※\t"; // Snaredrum
        else if (drum == Tom2)
            Globals::output_string[drum] = "!o\t"; // Tom 2
        else if (drum == Ride)
            Globals::output_string[drum] = "!xx\t"; // Ride
        else if (drum == Crash1)
            Globals::output_string[drum] = "!-X-\t"; // Crash
        else if (drum == Crash2)
            Globals::output_string[drum] = "!-XX-\t"; // Crash
        break;

        // case 5: // print swell_val for repeated MIDI notes in "swell" mode
        //   Globals::output_string[incoming_i] = swell_val[incoming_i];
        //   Globals::output_string[incoming_i] += "\t";
        //   break;
    }
}

// --------------------------------------------------------------------