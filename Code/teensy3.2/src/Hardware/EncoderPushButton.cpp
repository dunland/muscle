#include <Hardware.h>
#include <Song.h>
#include <Instruments.h>
#include <Calibration.h>
#include <settings.h>
#include <JSON.h>

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
        lcd->clear(); // clear when crossing 3-digit-number
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
  static bool last_button_state = false;
  static unsigned long last_button_toggle = 1000; // some pre-delay to prevent initial misdetection
  static unsigned long last_hold_trigger = 0;
  static bool readyToPush = false;

  bool button_state = !digitalRead(PUSHBUTTON);

  // ---------------------- BUTTON TOGGLE: ------------------
  if (button_state != last_button_state && millis() > last_button_toggle + 20) // button status changed
  {
    if (readyToPush)
    {

      switch (Globals::machine_state)
      {
      case Running:
        if (button_state == false) // button released
        {
          Globals::active_song->proceed_to_next_score();
        }
        break;

      case Calibrating:

        if (button_state == false && millis() > last_button_toggle + 200) // button released
        {
          // save current encoder value:
          encoder_value = encoder_count;

          // go one level down and setup new mode:
          Calibration::set(encoder_value);
        }

        break;

      default:
        Serial.println("no machine state for push button!");

        break;
      }
    }
    last_button_state = button_state;
    last_button_toggle = millis();
    readyToPush = true;
  }

  // -------------------- BUTTON HOLD: ----------------------
  else if (button_state == true && millis() > last_button_toggle + 1000 && millis() > 5000 && millis() > last_hold_trigger + 1000) // button pushed + held for 1000ms
  {
    switch (Globals::machine_state)
    {
    case Running:
      // go back one song:
      Globals::active_song_pointer--;
      if (Globals::active_song_pointer < 0)
        Globals::active_song_pointer = Globals::songlist.size() - 1;
      Globals::active_song = Globals::songlist[Globals::active_song_pointer];
      break;

    case Calibrating:
      switch (Calibration::calibration_mode)
      {
        // root menu level:
      case Select_Instrument:

        // leave calibration mode:
        noInterrupts();
        Globals::machine_state = Running;
        interrupts();

        // save settings to SD card:
        if (Globals::bUsingSDCard)
          JSON::save_settings_to_SD(Drumset::instruments);

        // clear up:
        Serial.println("leaving calibration mode.");
        Hardware::lcd->clear();
        break;

      // 1st menu level
      case Select_Sensitivity_Param: // ascend back to Instrument Selection
        Calibration::calibration_mode = Select_Instrument;
        Hardware::lcd->clear();
        break;

      // 2nd menu level
      case Set_Value:
        Calibration::calibration_mode = Select_Sensitivity_Param;
        Hardware::lcd->clear();
        break;

      default:
        break;
      }
      break;

    default:
      break;
    }
    last_hold_trigger = millis();
    readyToPush = false; // button needs to be pushed first before next action upon release
  }
}

bool Hardware::pushbutton_state()
{
  static unsigned long lastPush = 0;
  if (digitalRead(PUSHBUTTON) == LOW && millis() > lastPush + 200)
  {
    lastPush = millis();
    lcd->setCursor(11, 0);
    lcd->print("!");
    Serial.println("push!");
    return true;
  }
  else
  {
    return false;
  }
}

// --------------------------------------------------------------------