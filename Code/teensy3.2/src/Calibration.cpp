#include <Calibration.h>
#include <Instruments.h>
#include <Hardware.h>

Calibration_Mode Calibration::calibration_mode = Select_Instrument;
Instrument *Calibration::selected_instrument;
int Calibration::selected_sensitivity_param = SET_THRESHOLD;

int Calibration::recent_threshold_crossings = 0;

void Calibration::setup()
{
    selected_instrument = Drumset::instruments[0];
}

// set LCD menu using pushButton:
void Calibration::set(int val)
{
    Hardware::lcd->clear();
    switch (calibration_mode)
    {

    case Select_Instrument:
        calibration_mode = Select_Sensitivity_Param;
        Serial.println("changing calibration mode to select_sensitivity_param");

        break;

    case Select_Sensitivity_Param:
        calibration_mode = Set_Value;
        Hardware::encoder_maxVal = 1023;
        Serial.println("changing calibration mode to Set_Value");
        noInterrupts();
        if (selected_sensitivity_param == 0)
            Hardware::encoder_count = selected_instrument->sensitivity.threshold;
        else if (selected_sensitivity_param == 1)
            Hardware::encoder_count = selected_instrument->sensitivity.crossings;
        else if (selected_sensitivity_param == 2)
            Hardware::encoder_count = selected_instrument->sensitivity.delayAfterStroke;
        else if (selected_sensitivity_param == 3)
            Hardware::encoder_count = selected_instrument->timing.countAfterFirstStroke;
        interrupts();
        break;

    case Set_Value:
        calibration_mode = Select_Sensitivity_Param;
        Hardware::encoder_maxVal = 127;
        Serial.println("changing calibration mode back to Select_Sensitivty_Param");
        break;

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


    static int previous_encoder_count = -1;
    if (Hardware::encoder_count != previous_encoder_count)
    {
        Hardware::lcd->clear();
        switch (calibration_mode)
        {
        case Select_Instrument:
            selected_instrument = Drumset::instruments[Hardware::encoder_count % Drumset::instruments.size()];
            break;

        case Select_Sensitivity_Param:
            selected_sensitivity_param = Hardware::encoder_count % 4;
            break;

        case Set_Value:

            if (selected_sensitivity_param == 0) // Threshold
            {
                noInterrupts();
                Calibration::selected_instrument->sensitivity.threshold = Hardware::encoder_count;
                interrupts();
            }
            else if (selected_sensitivity_param == 1) // Crossings
            {
                noInterrupts();
                Calibration::selected_instrument->sensitivity.crossings = Hardware::encoder_count;
                interrupts();
            }
            else if (selected_sensitivity_param == 2) // Threshold
            // Delay_After_Stroke
            {

                noInterrupts();
                Calibration::selected_instrument->sensitivity.delayAfterStroke = Hardware::encoder_count;
                interrupts();
            }

            else if (selected_sensitivity_param == 3)
            {

                noInterrupts();
                Calibration::selected_instrument->timing.countAfterFirstStroke = Hardware::encoder_count % 2;
                interrupts();
            }
            break;

        default:
            Serial.println("no calibration mode set!");
            break;
        }
    }
    previous_encoder_count = Hardware::encoder_count;
}