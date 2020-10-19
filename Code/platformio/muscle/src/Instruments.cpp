#include <Instruments.h>
#include <MIDI.h>
#include <Tsunami.h>


////////////////////////////// CALCULATE NOISEFLOOR ///////////////////
///////////////////////////////////////////////////////////////////////
void Instrument::calculateNoiseFloor(Instrument* instrument)
{
  // calculates the average noise floor out of 400 samples from all inputs

  int led_idx = 0;
  
    Serial.print("calculating noiseFloor for ");
    Serial.print(Globals::DrumtypeToHumanreadable(instrument->drumtype));
    Serial.print(" ..waiting for stroke");
    if (Globals::use_responsiveCalibration)
    {
      while (analogRead(instrument->pin) < 700 + instrument->sensitivity.threshold)
        ; // calculate noiseFloor only after first stroke! noiseFloor seems to change with first stroke sometimes!
      Serial.print(" .");
      delay(1000); // should be long enough for drum not to oscillate anymore
    }

    int totalSamples = 0;
    boolean toggleState = false;
    for (int n = 0; n < 400; n++)
    {
      if (n % 100 == 0)
      {
        Serial.print(" . ");
        digitalWrite(Globals::leds[instrument->drumtype], toggleState);
        toggleState = !toggleState;
      }
      totalSamples += analogRead(instrument->pin);
    }
    instrument->sensitivity.noiseFloor = totalSamples / 400;
    digitalWrite(Globals::leds[instrument->drumtype], LOW);
    led_idx++;
    Serial.print("noiseFloor = ");
    Serial.println(instrument->sensitivity.noiseFloor);
  

  for (int i = 0; i < Globals::numInputs; i++) // turn LEDs off again
  {
    digitalWrite(Globals::leds[i], LOW);
    Globals::output_string[i] = "\t";
  }
}
// --------------------------------------------------------------------

void Instrument::trigger(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (effect)
    {
    case PlayMidi:
        Effect::playMidi(instrument, MIDI);
        break;

    case Monitor:
        Effect::monitor(instrument);
        break;

    case ToggleRhythmSlot:
        Effect::toggleRhythmSlot(instrument);
        break;

    case FootSwitchLooper:
        Effect::footswitch_recordSlots(instrument);
        break;

    case TapTempo:
        Effect::getTapTempo();
        break;

    case Swell:
        Effect::swell_rec(instrument, MIDI);
        break;

    case TsunamiLink:

        break;

    case CymbalSwell: // swell-effect for loudness on field recordings (use on cymbals e.g.)
        // TODO: UNTESTED! (2020-10-09)
        Effect::swell_rec(instrument, MIDI);
        break;
    case TopographyLog:

        break;

    default:
        break;
    }
}

///////////////////////////// TIMED EFFECTS ///////////////////////////
///////////////////////////////////////////////////////////////////////

void Instrument::perform(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    switch (effect)
    {
    case PlayMidi:
        break;

    case Monitor:
        break;

    case ToggleRhythmSlot:
        Effect::sendMidiNotes_timed(instrument, MIDI);
        break;

    case FootSwitchLooper:
        Effect::setInstrumentSlots(instrument);
        Effect::sendMidiNotes_timed(instrument, MIDI);
        break;

    case TapTempo:
        break;

    case Swell:
        Effect::swell_perform(instrument, MIDI);
        break;

    case TsunamiLink:

        break;

    case CymbalSwell:

        break;
    case TopographyLog:

        break;

    default:
        break;
    }
}