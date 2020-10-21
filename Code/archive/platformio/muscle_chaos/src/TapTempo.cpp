#include <Arduino.h>
#include <Globals.h>
#include <TapTempo.h>

void TapTempo::getTapTempo()
    {
      static unsigned long timeSinceFirstTap = 0;
      static int tapState = 1;
      static int num_of_taps = 0;
      static int clock_sum = 0;

      switch (tapState)
      {
        //    case 0: // this is for activation of tap tempo listen
        //      tapState = 1;
        //      break;

      case 1:                                     // first hit
        if (millis() > timeSinceFirstTap + 10000) // reinitiate tap if not used for ten seconds
        {
          num_of_taps = 0;
          clock_sum = 0;
          Serial.println("-----------TAP RESET!-----------\n");
        }
        timeSinceFirstTap = millis(); // record time of first hit
        tapState = 2;                 // next: wait for second hit

        break;

      case 2: // second hit

        if (millis() < timeSinceFirstTap + 2000) // only record tap if interval was not too long
        {
          num_of_taps++;
          clock_sum += millis() - timeSinceFirstTap;
          tapInterval = clock_sum / num_of_taps;
          Serial.print("new tap Tempo is ");
          Serial.print(60000 / tapInterval);
          Serial.print(" bpm (");
          Serial.print(tapInterval);
          Serial.println(" ms interval)");

          current_BPM = 60000 / tapInterval;
          tapState = 1;

          masterClock.begin(masterClockTimer, tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM
        }

        if (timeSinceFirstTap > 2000) // forget tap if time was too long
        {
          tapState = 1;
          // Serial.println("too long...");
        }
        // }
        break;
      }
    }
  }