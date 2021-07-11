#include <Hardware.h>
#include <Score/Score.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>

///////////////////////////// ROTARY ENCODER //////////////////////////
///////////////////////////////////////////////////////////////////////
Encoder *Hardware::myEnc = new Encoder(ENCODER1, ENCODER2);
int Hardware::encoder_value;
int Hardware::encoder_count = 0;
int Hardware::encoder_maxVal = 127;

void Hardware::checkEncoder()
{
  static long encoder_oldPosition = -999;
  static int encoder_previous_count = 0;
  long encoder_newPosition = myEnc->read();
  static unsigned long last_encoder_change = 0;

  // ROTARY ENCODER:
  if (encoder_oldPosition != encoder_newPosition && millis() > last_encoder_change + ENCODER_TIMEOUT)
  {

    // rising value:
    if (encoder_oldPosition > encoder_newPosition)
    {
      if (encoder_count > 0)
        encoder_count--;
      encoder_oldPosition = encoder_newPosition;
      if (encoder_count < 100 && encoder_previous_count >= 100)
        lcd->clear();
      encoder_previous_count = encoder_count;
    }

    // sinking value:
    else
    {
      if (encoder_count < encoder_maxVal)
        encoder_count++;
      encoder_oldPosition = encoder_newPosition;
    }
    last_encoder_change = millis();
  }
}

///////////////////////////// PUSH BUTTON ///////////////////////////
///////////////////////////////////////////////////////////////////////
unsigned long Hardware::last_pushbutton_release = 0;

void Hardware::checkPushButton()
{
  static int button_state;
  static int last_button_state = HIGH;
  static unsigned long last_button_toggle = 1000; // some pre-delay to prevent initial misdetection
  static unsigned long last_level_ascend = 0;

  button_state = digitalRead(PUSHBUTTON);
  if (button_state != last_button_state && millis() > last_button_toggle + 20 && millis() > last_level_ascend + 3000) // button status changed
  {
    switch (Globals::machine_state)
    {
    case Machine_Running:
      if (button_state == HIGH) // button released
      {
        Globals::active_score->proceed_to_next_score();
        Serial.println("push button pressed.");
      }
      break;

    case Machine_Calibrating:

      if (button_state == HIGH) // button released
      {
        encoder_value = encoder_count;
        Serial.println("push button pressed.");
        Calibration::set(encoder_value);
      }

      break;

    default:
      Serial.println("no machine state for push button!");

      break;
    }
    last_button_state = button_state;
    last_button_toggle = millis();
  }

  // leave calibration mode
  if (button_state == LOW && millis() > last_button_toggle + 3000 && millis() > 10000 && millis() > last_level_ascend + 3000) // pressed for 3s
  {
    switch (Calibration::calibration_mode)
    {
    case Select_Instrument:
      noInterrupts();
      Globals::machine_state = Machine_Running;
      interrupts();
      // TODO: save settings to SD card
      Serial.println("leaving calibration mode.");
      Hardware::lcd->clear();
      break;

    case Select_Sensitivity_Param: // ascend back to Instrument Selection
      Calibration::calibration_mode = Select_Instrument;
      Hardware::lcd->clear();
      break;

    case Set_Value:
      Calibration::calibration_mode = Select_Sensitivity_Param;
      Hardware::lcd->clear();
      break;

    default:
      break;
    }
    last_level_ascend = millis();
    last_button_state = HIGH;
  }
}

// boolean Hardware::pushbutton_is_pressed()
// {
//   static unsigned long lastPush = 0;
//   if (digitalRead(PUSHBUTTON) == LOW && millis() > lastPush + 200)
//   {
//     lastPush = millis();
//     lcd->setCursor(11, 0);
//     lcd->print("!");
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }

// --------------------------------------------------------------------
