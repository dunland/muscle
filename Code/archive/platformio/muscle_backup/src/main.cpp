/*
   ------------------------------------
   September 2020
   by David Unland david[at]unland[dot]eu
   ------------------------------------
   ------------------------------------
  0. calibrate sensors
  1. set calibration[instrument][0/1] values. (in setup())
    0:threshold, 1:min num of threshold crossings
  2. set mode for instruments:

    pinActions:
    -----------
    0 = play MIDI note upon stroke
    1 = binary beat logger (print beat)
    2 = toggle rhythm_slot
    3 = footswitch looper: records what is being played for one bar while footswitch is pressed and repeats it after release.
    4 = tapTempo: a standard tapTempo to change the overall pace. to be used on one instrument only.
    5 = "swell" effect: all instruments have a tap tempo that will change MIDI CC values when played a lot (values decrease automatically)
    6 = Tsunami beat-linked playback: finds patterns within 1 bar for each instrument and plays an according rhythmic sample from tsunami database
    7 = using swell effect for tsunami playback loudness (arhythmic field recordings for cymbals)
    8 = 16th-note-topography with MIDI playback and volume change
*/

/* --------------------------------------------------------------------- */
/* ------------------------------- GLOBAL ------------------------------ */
/* --------------------------------------------------------------------- */
#include <Arduino.h>
#include <Globals.h>
#include <Functions.h>


// ----------------------------- pins ---------------------------------

// static const uint8_t leds[] = {LED_BUILTIN, LED_BUILTIN1, LED_BUILTIN2, LED_BUILTIN3}; // array when using SPRESENSE


/* --------------------------------------------------------------------- */
/* ---------------------------------- SETUP ---------------------------- */
/* --------------------------------------------------------------------- */

void setup()
{

  Serial.begin(115200);
  // Serial3.begin(57600); // contained in tsunami.begin()
  while (!Serial)
    ; // prevents Serial flow from just stopping at some (early) point.
  // delay(1000); // alternative to line above, if run with external power (no computer)

  MIDI.begin(MIDI_CHANNEL_OMNI);

  delay(1000);     // wait for Tsunami to finish reset // redundant?
  Hardware::tsunami.start(); // Tsunami startup at 57600. ATTENTION: Serial Channel is selected in Tsunami.h !!!
  delay(100);
  Hardware::tsunami.stopAllTracks(); // in case Tsunami was already playing.
  Hardware::tsunami.samplerateOffset(0, 0);
  Hardware::tsunami.setReporting(true); // Enable track reporting from the Tsunami
  delay(100);                 // some time for Tsunami to respond with version string

  //------------------------ initialize pins and arrays ------------------------
  for (int i = 0; i < Globals::numInputs; i++)
  {
    pinMode(Globals::leds[i], OUTPUT);
    Timing::counts[i] = 0;
    for (int j = 0; j < 8; j++)
    {
      Score::read_rhythm_slot[i][j] = false;
      Score::set_rhythm_slot[i][j] = false;
      Score::beat_topography_8.array[i][j] = 0;
      Score::beat_topography_16.array[i][j] = 0;
    }
    Score::initialPinAction[i] = Globals::pinAction[i];
  }
  swell_init();
  pinMode(VIBR, OUTPUT);
  pinMode(FOOTSWITCH, INPUT_PULLUP);

  // ---------------------------------------------------------------------------
  // instrument naming for human-readable console outputs

  // calculate noise floor:
  calculateNoiseFloor();

  // setup initial values ---------------------------------------------

  // set instrument calibration array
  // values as of 2020-08-27:
  Calibration::calibration[Snare][0] = 180;
  Calibration::calibration[Snare][1] = 12;
  Calibration::calibration[Hihat][0] = 70; // 60
  Calibration::calibration[Hihat][1] = 30; // 20
  Calibration::calibration[Kick][0] = 100;
  Calibration::calibration[Kick][1] = 16;
  Calibration::calibration[Tom1][0] = 200;
  Calibration::calibration[Tom1][1] = 20;
  Calibration::calibration[Standtom1][0] = 70;
  Calibration::calibration[Standtom1][1] = 12;
  Calibration::calibration[Tom2][0] = 300;
  Calibration::calibration[Tom2][1] = 18;
  Calibration::calibration[Cowbell][0] = 80;
  Calibration::calibration[Cowbell][1] = 15;

  Serial.println("-----------------------------------------------");
  Serial.println("calibration values set as follows:");
  Serial.println("instr\tthrshld\tcrosses\tnoiseFloor");
  for (int i = 0; i < Globals::numInputs; i++)
  {
          // Serial.print(DrumtypeToHumanreadable(DrumType(i)));
    Serial.print("\t");
    for (int j = 0; j < 2; j++)
    {
      Serial.print(Calibration::calibration[i][j]);
      Serial.print("\t");
    }
    Serial.println(Calibration::noiseFloor[i]);
  }
  Serial.println("-----------------------------------------------");

  //  start timer -----------------------------------------------------

  Timing::pinMonitor.begin(Timing::samplePins, 1000);                                // sample pin every 1 millisecond
  Timing::masterClock.begin(Timing::masterClockTimer, Timing::tapInterval * 1000 * 4 / 128); // 4 beats (1 bar) with 128 divisions in microseconds; initially 120 BPM

  // Debug:
  // Hardware::tsunami.trackPlayPoly(1, 0, true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
  // tracknum, channel
}

/* --------------------------------------------------------------------- */
/* ------------------------------- LOOP -------------------------------- */
/* --------------------------------------------------------------------- */

void loop()
{
  static int current_eighth_count = 0; // overflows at current_beat_pos % 8
  static int current_16th_count = 0;   // overflows at current_beat_pos % 2
  static int last_eighth_count = 0;    // stores last eightNoteCount for comparison
  static int last_16th_count = 0;      // stores last eightNoteCount for comparison
  static unsigned long lastNotePlayed[Globals::numInputs];

  Hardware::tsunami.update();

  // ------------------------- DEBUG AREA -----------------------------
  printNormalizedValues(Debug::printNormalizedValues_);

  // --------------------- INCOMING SIGNALS FROM PIEZOS ---------------
  // (define what should happen when instruments are hit)
  for (int i = 0; i < Globals::numInputs; i++)
  {
    if (stroke_detected(i)) // evaluates pins for activity repeatedly
    {
      // ----------------------- perform pin action -------------------
      switch (Globals::pinAction[i])
      {
      case 0:
        MIDI.sendNoteOn(Score::notes_list[i], 127, 2);
        lastNotePlayed[i] = millis();
        break;

      case 1: // monitor: just print what is being played.
        if (Debug::printStrokes)
        {
          setInstrumentPrintString(i, Globals::pinAction[i]);
        }
        break;

      case 2: // toggle beat slot
        if (Debug::printStrokes)
          setInstrumentPrintString(i, Globals::pinAction[i]);
        Score::read_rhythm_slot[i][current_eighth_count] = !Score::read_rhythm_slot[i][current_eighth_count];
        break;

      case 3: // record what is being played and replay it later
        if (Debug::printStrokes)
          setInstrumentPrintString(i, Globals::pinAction[i]);
        Score::set_rhythm_slot[i][current_eighth_count] = true;
        break;

      case 4: // tapTempo
        getTapTempo();
        break;

      case 5: // "swell"
        setInstrumentPrintString(i, Globals::pinAction[i]);
        swell_rec(i);
        break;

      case 6: // Tsunami beat-linked pattern
        setInstrumentPrintString(i, 1);
        Score::beat_topography_8.array[i][current_eighth_count]++;
        break;

      case 7: // swell-effect for loudness on field recordings (use on cymbals e.g.)
        // TODO: UNTESTED! (2020-10-09)
        setInstrumentPrintString(i, Globals::pinAction[i]);
        swell_rec(i);
        break;

      case 8: // create beat-topography in 16-th
        setInstrumentPrintString(i, 1);
        Score::beat_topography_16.array[i][current_16th_count]++;
        break;

      default:
        break;
      }

      // send instrument stroke to processing:
      // send_to_processing(i);
    }
  } // end main commands loop -----------------------------------------

  /////////////////////////// TIMED ACTIONS ///////////////////////////
  /////////////////////////////////////////////////////////////////////
  // (automatically invoke rhythm-linked actions)
  static int last_beat_pos = 0;
  static boolean toggleLED = true;
  static boolean sendMidiCopy = false;

  static unsigned long vibration_begin = 0;
  static int vibration_duration = 0;

  static boolean already_printed = false;

  noInterrupts();
  Timing::current_beat_pos = Timing::beatCount % 32; // (beatCount increases infinitely)
  sendMidiCopy = Timing::sendMidiClock;      // MIDI flag for Clock to be sent
  interrupts();

  // ---------------------------- send MIDI-Clock on beat
  /* MIDI Clock events are sent at a rate of 24 pulses per quarter note
     one tap beat equals one quarter note
     only one midi clock signal should be send per 24th quarter note
  */
  if (sendMidiCopy)
  {
    Serial2.write(0xF8); // MIDI-clock has to be sent 24 times per quarter note
    noInterrupts();
    Timing::sendMidiClock = false;
    interrupts();
  }

  // DO THINGS ONCE PER 32nd-STEP: ------------------------------------
  // ------------------------------------------------------------------
  if (Timing::current_beat_pos != last_beat_pos)
  {
    // increase 8th note counter:
    if (Timing::current_beat_pos % 4 == 0)
    {
      current_eighth_count = (current_eighth_count + 1) % 8;
      toggleLED = !toggleLED;
    }
    digitalWrite(LED_BUILTIN, toggleLED);

    // increase 16th note counter:
    if (Timing::current_beat_pos % 2 == 0)
    {
      current_16th_count = (current_16th_count + 1) % 16;
    }

    // -------------------------- PIN ACTIONS: ------------------------
    // ----------------------------------------------------------------
    for (int i = 0; i < Globals::numInputs; i++)
    {
      // --------------------------- SWELL: ---------------------------
      if (Globals::pinAction[i] == 5 || Globals::pinAction[i] == 7)
        swell_perform(i, Globals::pinAction[i]); // ...updates once a 32nd-beat-step

      else if (Globals::pinAction[i] == 3) // set rhythm slots to play MIDI notes:
        Score::read_rhythm_slot[i][current_eighth_count] = Score::set_rhythm_slot[i][current_eighth_count];

      // ----------- (pinActions 2 and 3): send MIDI notes ------------
      else if (Globals::pinAction[i] == 2 || Globals::pinAction[i] == 3)
      {
        if (current_eighth_count != last_eighth_count) // in 8th-interval
        {
          if (Score::read_rhythm_slot[i][current_eighth_count])
          {
            setInstrumentPrintString(i, 3);
            MIDI.sendNoteOn(Score::notes_list[i], 127, 2);
          }
          else
            MIDI.sendNoteOff(Score::notes_list[i], 127, 2);
        }
      }

      // ----------------------- TSUNAMI PLAYBACK: --------------------
      else if (Globals::pinAction[i] == 6)
      {
        tsunami_beat_playback(i, Timing::current_beat_pos);
      }

      // ---------- MIDI playback according to beat_topography --------
      if (Globals::pinAction[i] == 8)
      {
        if (current_16th_count != last_16th_count) // do this only once per 16th step
        {

          // create overall volume topography of all instrument layers:
          int total_vol[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

          for (int idx = 0; idx < 16; idx++)
          {
            for (int instr = 0; instr < Globals::numInputs; instr++)
            {
              if (Globals::pinAction[instr] == 8)
                total_vol[idx] += Score::beat_topography_16.array[instr][idx];
            }
          }

          // smoothen array:
          // ------------------ create topography and smoothen it -------------
          smoothen_dataArray(total_vol, 3);

          // print volume layer:
          if (!already_printed)
          {
            Serial.print("vol:\t[");

            for (int j = 0; j < 16; j++)
            {
              Serial.print(total_vol[j]);
              if (j < 15)
                Serial.print(",");
            }
            Serial.println("]");
            already_printed = true;
          }


          // ------------ result: change volume and play MIDI ---------
          // ----------------------------------------------------------
          int vol = min(40 + total_vol[current_16th_count] * 15, 255);

          if (Score::beat_topography_16.array[i][current_16th_count] > 0)
          {
            MIDI.sendControlChange(50, vol, 2);
            MIDI.sendNoteOn(Score::notes_list[i], 127, 2);
          }
          else
          {
            MIDI.sendNoteOff(Score::notes_list[i], 127, 2);
          }

          // Debug:
          // Serial.print(DrumtypeToHumanreadable(DrumType(i)));
          Serial.print(":\t[");
          for (int j = 0; j < 16; j++)
          {
            Serial.print(Score::beat_topography_16.array[i][j]);
            if (j < 15)
              Serial.print(",");
          }
          Serial.println("]");
        } // end only once per 16th-step
      } // end pinAction[8]
    } // end pin Actions
    // ----------------------------------------------------------------

        ///////////////////////////////////////////////////////////////////
    /////////////////////////// ABSTRACTIONS //////////////////////////
    ///////////////////////////////////////////////////////////////////

    //makeTopo();
    // works like this:
    // smoothen_dataArray(&Score::beat_topography_16);
    // LOTS OF PSEUDOCODE HERE:

    //    Score::beat_topography_16.smoothen(); // → beat_topography.average_smoothened_height
    //
    //    if (Score::beat_topography_16.average_smoothened_height >= beat_topography.threshold)
    //    {
    //      // create next abstraction layer
    //      int[] beat_regularity = new int[16];
    //
    //      // update abstraction layer:
    //      for (int i = 0; i < 16; i++)
    //        beat_regularity[i] = Score::beat_topography_16[i];
    //
    //      // get average height again:
    //      beat_regularity.smoothen();
    //    }
    //
    //    if (beat_regularity.amplitude >= beat_regularity.threshold)
    //    {
    //      // you are playing super regularly. that's awesome. now activate next element in score
    //      score_next_element_ready = true;
    //    }
    //
    //    // if regularity is held for certain time (threshold reached):
    //    read_to_break
    //
    //    when regularity expectations not fulfilled:
    //    score: go to next element
    //
    //    // elsewhere:
    //    if (hit_certain_trigger)
    //    {
    //
    //      if (score_next_element_ready)
    //      {
    //        /* do whatever is up next in the score */
    //      }
    //    }

    /* Score could look like this:
      intro----------part 1--...-----outro--------
      1     2     3     4    ...     20    21     step
      ++    ++    ++                              element_FX
                ++    ++   ...     ++    ++     element_notes
                      ++           ++           element_fieldRecordings

      cool thing: create score dynamically according to how I play
    */

    // PSEUDOCODE END


    /////////////////////// AUXILIARY FUNCTIONS ///////////////////////
    // ---------------------------- vibrate on beat:
    if (Timing::current_beat_pos % 8 == 0) // current_beat_pos holds 32 → %8 makes 4.
    {
      vibration_begin = millis();
      vibration_duration = 50;
      digitalWrite(VIBR, HIGH);
    }

    // ----------------------------- draw play log to console
    print_to_console(String(millis()));
    print_to_console("\t");
    // Serial.print(current_eighth_count + 1); // if you want to print 8th-steps only
    print_to_console(Timing::current_beat_pos);
    print_to_console("\t");
    // Serial.print(Timing::current_beat_pos / 4);
    // Serial.print("\t");
    // Serial.print(current_eighth_count);
    for (int i = 0; i < Globals::numInputs; i++)
    {
      print_to_console(Debug::output_string[i]);
      Debug::output_string[i] = "\t"; // prepare array with empty entries
    }
    println_to_console("");

    // Debug: play MIDI note on quarter notes
    //    if (Timing::current_beat_pos % 8 == 0)
    //    MIDI.sendNoteOn(57, 127, 2);
    //    else
    //    MIDI.sendNoteOff(57, 127, 2);

  } // end of (32nd-step) TIMED ACTIONS
  // ------------------------------------------------------------------

  ///////////////////////////// tidy up ///////////////////////////////
  last_beat_pos = Timing::current_beat_pos;
  last_eighth_count = current_eighth_count;
  last_16th_count = current_16th_count;
  already_printed = false;

  // check footswitch -------------------------------------------------
  checkFootSwitch();

  // turn off vibration and MIDI notes --------------------------------
  if (millis() > vibration_begin + vibration_duration)
    digitalWrite(VIBR, LOW);

  for (int i = 0; i < Globals::numInputs; i++)
    if (millis() > lastNotePlayed[i] + 200 && Globals::pinAction[i] != 5) // pinAction 5 turns notes off in swell_beat()
      MIDI.sendNoteOff(Score::notes_list[i], 127, 2);
}
// --------------------------------------------------------------------
