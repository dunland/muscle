#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>

////////////////////////////////// LCD ////////////////////////////////
///////////////////////////////////////////////////////////////////////
LiquidCrystal *Hardware::lcd = new LiquidCrystal(RS, EN, D4, D5, D6, D7);
volatile boolean Hardware::FLAG_CLEAR_LCD = false;

// ------------------------------------------------------------------------------
void Hardware::lcd_display()
{
    // clear LCD display three times per second:
    static unsigned long last_clear = 0;
    if (millis() > last_clear + 333)
    {
        lcd->clear();
        last_clear = millis();
    }
    // ------------- Machine-State-dependent display: -------------------
    switch (Globals::machine_state)
    {
    // -------------------------- RUNNING -------------------------------
    case Running:
    {
        // switch if any instrument has CC mode:
        int instruments_with_CC_mode = 0;
        for (uint8_t i = 0; i < Drumset::instruments.size(); i++)
        {
            if (Drumset::instruments[i]->effect == Change_CC)
            {
                instruments_with_CC_mode++;
            }
        }

        // start to toggle when CC-occupied display area gets too large
        if (instruments_with_CC_mode > 3)
        {
            static unsigned long last_running_state_switch = 0;
            static boolean running_mode = true;

            if (millis() > (last_running_state_switch + 3000))
            {
                running_mode = !running_mode;
                lcd->clear();
                last_running_state_switch = millis();
            } ///////////////////////////////////////////////////////////////////////

            if (running_mode == true) // mode A: display scores
                display_scores();
            else // mode B: display Midi Values
                display_Midi_values();
        }

        // display both score and midi vals:
        else
        {
            display_scores();
            display_Midi_values();
        }
    }
    break;

    // -------------------------- CALIBRATION ---------------------------
    case Calibrating:
    {
        switch (Calibration::calibration_mode)
        {
        // level 1: display instrument with sensitivity values
        case Select_Instrument:

            // display instrument:
            lcd->setCursor(0, 0);
            lcd->print("[");
            lcd->setCursor(1, 0);
            lcd->print(Globals::DrumtypeToHumanreadable(Calibration::selected_instrument->drumtype));
            lcd->setCursor(7, 0);
            lcd->print("]");
            lcd->setCursor(12, 0);
            lcd->print("n");
            lcd->setCursor(13, 0);
            lcd->print(Calibration::selected_instrument->sensitivity.noiseFloor);

            // display instrument values:
            lcd->setCursor(0, 1);
            lcd->print("t");
            lcd->setCursor(1, 1);
            lcd->print(Calibration::selected_instrument->sensitivity.threshold);
            lcd->setCursor(4, 1);
            lcd->print("c");
            lcd->setCursor(5, 1);
            lcd->print(Calibration::selected_instrument->sensitivity.crossings);
            lcd->setCursor(8, 1);
            lcd->print("d");
            lcd->setCursor(9, 1);
            lcd->print(Calibration::selected_instrument->sensitivity.delayAfterStroke);
            lcd->setCursor(12, 1);
            if (Calibration::selected_instrument->timing.countAfterFirstStroke == true)
                lcd->print("true");
            else
                lcd->print("false");

            break;

        case Select_Sensitivity_Param:

            // lcd->autoscroll();
            lcd->setCursor(0, 0);
            if (Calibration::selected_sensitivity_param == 0)
                lcd->print("[Threshold]");
            else if (Calibration::selected_sensitivity_param == 1)
                lcd->print("[Crossings]");
            else if (Calibration::selected_sensitivity_param == 2)
                lcd->print("[strkDelay]");
            else if (Calibration::selected_sensitivity_param == 3)
                lcd->print("[FirstStrk]");
            else
                lcd->print(Calibration::selected_sensitivity_param);

            lcd->setCursor(12, 0);
            if (Calibration::selected_sensitivity_param == 0)
                lcd->print(Calibration::selected_instrument->sensitivity.threshold);
            else if (Calibration::selected_sensitivity_param == 1)
                lcd->print(Calibration::selected_instrument->sensitivity.crossings);
            else if (Calibration::selected_sensitivity_param == 2)
                lcd->print(Calibration::selected_instrument->sensitivity.delayAfterStroke);
            else if (Calibration::selected_sensitivity_param == 3)
                if (Calibration::selected_instrument->timing.countAfterFirstStroke)
                    lcd->print("true");
                else
                    lcd->print("false");
            else
                lcd->print(Calibration::selected_sensitivity_param);

            lcd->setCursor(0, 1);
            lcd->print("counts:");
            lcd->setCursor(8, 1);
            lcd->print(Calibration::recent_threshold_crossings);

            break;

        case Set_Value:

            // lcd->autoscroll();
            lcd->setCursor(0, 0);
            if (Calibration::selected_sensitivity_param == 0)
                lcd->print("Threshold");
            if (Calibration::selected_sensitivity_param == 1)
                lcd->print("Crossings");
            if (Calibration::selected_sensitivity_param == 2)
                lcd->print("strkDelay");
            if (Calibration::selected_sensitivity_param == 3)
                lcd->print("firstSrk");

            lcd->setCursor(11, 0);
            lcd->print("[");
            lcd->setCursor(12, 0);

            if (Calibration::selected_sensitivity_param == 0)
                lcd->print(Calibration::selected_instrument->sensitivity.threshold);
            if (Calibration::selected_sensitivity_param == 1)
                lcd->print(Calibration::selected_instrument->sensitivity.crossings);
            if (Calibration::selected_sensitivity_param == 2)
                lcd->print(Calibration::selected_instrument->sensitivity.delayAfterStroke);
            if (Calibration::selected_sensitivity_param == 3)
                if (Calibration::selected_instrument->timing.countAfterFirstStroke)
                    lcd->print("true");

            lcd->setCursor(15, 0);
            lcd->print("]");

            lcd->setCursor(0, 1);
            lcd->print("counts:");
            lcd->setCursor(8, 1);
            lcd->print(Calibration::recent_threshold_crossings);

            break;

        default:
            Serial.println("no mode!");

            break;
        }
    }
    break;

    default:
        Serial.println("no machine state!");

        break;
    }
}

// ------------------------------------------------------------------------------
void Hardware::display_scores()
{
    // active score display:
    lcd->setCursor(0, 1);
    lcd->print(Globals::songlist[Globals::active_song_pointer]->name);

    lcd->setCursor(14, 1);
    lcd->print(Globals::active_song->step);
}

// ------------------------------------------------------------------------------
// display midi values of instruments with FX-Type CC_Change
void Hardware::display_Midi_values()
{
    for (uint8_t i = 0; i < Drumset::instruments.size(); i++)
    {
        if (Drumset::instruments[i]->effect == Change_CC)
        {
            Hardware::lcd->setCursor(((i % 4) * 4), int(i >= 4));
            Hardware::lcd->print(Globals::DrumtypeToHumanreadable(Drumset::instruments[i]->drumtype)[0]);
            Hardware::lcd->setCursor(((i % 4) * 4) + 1, int(i >= 4));
            Hardware::lcd->print(int(Drumset::instruments[i]->midi_settings.cc_val));
        }
    }
}

// --------------------------------------------------------------------