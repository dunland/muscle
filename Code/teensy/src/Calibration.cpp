#include <Calibration.h>
#include <Instruments.h>
#include <Hardware.h>

Calibration_Mode Calibration::calibration_mode = Select_Instrument;
Instrument *Calibration::selected_instrument;
int Calibration::selected_sensitivity_param = SET_THRESHOLD;

void Calibration::set(int val)
{
    switch (calibration_mode)
    {

    case Select_Instrument:
        calibration_mode = Select_Sensitivity_Param;
        break;

    case Select_Sensitivity_Param:
        calibration_mode = Set_Value;
        Hardware::encoder_maxVal = 1023;
        break;

    case Set_Value:
        calibration_mode = Select_Sensitivity_Param;
        Hardware::encoder_maxVal = 127;
        break;

    // case Set_Delay_After_Stroke:
    //     selected_instrument->sensitivity.delayAfterStroke = val;
    //     break;

    // case Set_Stroke_Detection_After_First:
    //     if (val == 0)
    //         selected_instrument->timing.countAfterFirstStroke = false;
    //     else
    //         selected_instrument->timing.countAfterFirstStroke = true;
    //     break;

    default:
        break;
    }
}

void Calibration::update()
{
    if (Hardware::FLAG_CLEAR_LCD)
    {
        Hardware::lcd->clear();
        noInterrupts();
        Hardware::FLAG_CLEAR_LCD = false;
        interrupts();
    }

    switch (calibration_mode)
    {
    case Select_Instrument:
        selected_instrument = Drumset::instruments[Hardware::encoder_count % Drumset::instruments.size()];
        break;

    case Select_Sensitivity_Param:
        selected_sensitivity_param = Hardware::encoder_count % 4;
        break;

    case Set_Value:

        switch (selected_sensitivity_param)
        {
        case 0: // Threshold
            noInterrupts();
            Calibration::selected_instrument->sensitivity.threshold = Hardware::encoder_value;
            interrupts();
            break;

       case 1: // Crossings
            noInterrupts();
            Calibration::selected_instrument->sensitivity.crossings = Hardware::encoder_value;
            interrupts();
            break;

       case 2: // Delay_After_Stroke
            noInterrupts();
            Calibration::selected_instrument->sensitivity.delayAfterStroke = Hardware::encoder_value;
            interrupts();
            break;

       case 3: // Threshold
            noInterrupts();
            Calibration::selected_instrument->timing.countAfterFirstStroke = Hardware::encoder_value % 2;
            interrupts();
            break;

        default:
            break;
        }

        break;

    default:
        break;
    }
}