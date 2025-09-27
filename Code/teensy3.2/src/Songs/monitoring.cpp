#include <Song.h>

#include <Hardware.h>
#include <Instruments.h>

///////////////////////////// MONITORING //////////////////////////////
void run_monitoring() {
  switch (Globals::active_song->step) {
  case 0:
    if (Globals::active_song->isStateInit()) {

      Synthesizers::kaossPad3->sendControlChange(
          92,
          0); // Touch Pad off - JUST BECAUSE ALHAMBRA IS BEFORE!// TODO:
              // execute this when leaving the song with push button! at best,
              // by using callback functions for song.proceed_to_next_score

      Synthesizers::whammy->sendProgramChange(83); // Whammy off!!

      Hardware::footswitch_mode = Increment_Score;
      Globals::active_song->resetInstruments();
      Globals::active_song->notes.clear();

      // turn off all currently playing MIDI notes:
      for (int channel = 1; channel < 3; channel++) {
        for (auto &synth : Synthesizers::synths)
          for (int note_number = 0; note_number < 127; note_number++) {
            synth->sendNoteOff(note_number);
          }
      }
    }

    Hardware::lcd->setCursor(0, 0);
    Hardware::lcd->print(Globals::current_BPM);
    Hardware::lcd->setCursor(3, 0);
    Hardware::lcd->print("BPM");

    break;

  default:
    Globals::active_song->proceed_to_next_score();
    break;
  }
}

void run_tapTempo() {
  switch (Globals::active_song->step) {
  case 0:
    if (Globals::active_song->isStateInit()) {
      Globals::active_song->resetInstruments();
      Globals::active_song->notes.clear();

      Drumset::hihat->set_effect(TapTempo);

      // turn off all currently playing MIDI notes:
      for (int channel = 1; channel < 3; channel++) {
        for (auto &synth : Synthesizers::synths)
          for (int note_number = 0; note_number < 127; note_number++) {
            synth->sendNoteOff(note_number);
          }
      }
    }

    Hardware::lcd->setCursor(0, 0);
    Hardware::lcd->print(Globals::current_BPM);
    Hardware::lcd->setCursor(3, 0);
    Hardware::lcd->print("BPM");

    break;

  default:
    Globals::active_song->proceed_to_next_score();
    break;
  }
}