#include <Calibration.h>
#include <Instruments.h>

Feature_Type Calibration::current_feature = Set_Threshold;
Instrument *Calibration::selected_instrument;

void Calibration::set_value(int val)
{
    switch (current_feature)
    {

    case Select_Instrument:
        selected_instrument = Drumset::instruments[val % Drumset::instruments.size()];
        break;

    case Set_Threshold:
        selected_instrument->sensitivity.threshold = val;
        break;

    case Set_Crossings:
        selected_instrument->sensitivity.crossings = val;
        break;

    case Set_Delay_After_Stroke:
        selected_instrument->sensitivity.delayAfterStroke = val;
        break;

    case Set_Stroke_Detection_After_First:
        if (val == 0)
            selected_instrument->timing.countAfterFirstStroke = false;
        else
            selected_instrument->timing.countAfterFirstStroke = true;
        break;

    default:
        break;
    }
}