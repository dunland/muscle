#include <Arduino.h>
#include <Globals.h>
#include <Core.h>
#include <Timing.h>

// ------------------------ get normalized pin value ------------------
int Core::pinValue(int pinVal_pointer_in, int noiseFloor)
{
  return abs(noiseFloor[pinVal_pointer_in] - analogRead(Globals::pins[pinVal_pointer_in]));
}

// --------------------------- stroke detection -----------------------
boolean Core::stroke_detected(int pinDect_pointer_in)
{
    static unsigned long lastPinActiveTimeCopy[Globals::numInputs];
    //static unsigned long firstPinActiveTimeCopy[Globals::numInputs];
    //static int lastValue[Globals::numInputs]; // for LED toggle
    //static boolean toggleState = false;       // for LED toggle

    noInterrupts();
    lastPinActiveTimeCopy[pinDect_pointer_in] = Timing::lastPinActiveTime[pinDect_pointer_in];
    //firstPinActiveTimeCopy[pinDect_pointer_in] = Timing::firstPinActiveTime[pinDect_pointer_in];
    interrupts();

    if (millis() > lastPinActiveTimeCopy[pinDect_pointer_in] + calibration->globalDelayAfterStroke) // get counts only X ms after LAST hit

    //if (millis() > firstPinActiveTimeCopy[pinDect_pointer_in] + globalDelayAfterStroke)
    //get counts only X ms after FIRST hit ??
    {
        static int countsCopy;
        noInterrupts();
        countsCopy = Timing::counts[pinDect_pointer_in];
        Timing::counts[pinDect_pointer_in] = 0;
        interrupts();

        // ---------------------------- found significant count!
        if (countsCopy >= calibration->calibration[pinDect_pointer_in][1])
        {
            // LED blink:
            //if (countsCopy != lastValue[pinDect_pointer_in]) toggleState = !toggleState;
            //digitalWrite(LED_BUILTIN, toggleState);
            //lastValue[pinDect_pointer_in] = countsCopy;

            // countsCopy = 0;

            return true;
        }
        else // timing ok but no significant counts
        {
            return false;
        }
    }
    else // TODO: timing not ok (obsolete: will always be ok!)
    {
        {
            return false;
        }
    }
}
// --------------------------------------------------------------------