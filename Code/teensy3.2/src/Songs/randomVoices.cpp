#include <Song.h>

#include "Devtools.h"
#include "Globals.h"
#include "Notes.h"
#include <Arduino.h>
#include <Hardware.h>
#include <Instruments.h>
#include <cstdint>

// Modi: // TODO: move all this to Notes.h or somewhere.
// didnt quite work when putting this to a header file, and did also not work in Globals.cpp.. tja
int dur[3] = {0, 4, 3};
int moll[3] = {0, 3, 4};
int ionisch[7] = {0, 2, 4, 5, 7, 9, 11};
int dorisch[7] = {0, 2, 3, 5, 7, 9, 10};
int phrygian[7] = {0, 1, 3, 5, 7, 9, 11};
int lydian[7] = {0, 2, 4, 6, 7, 9, 11};
int mixolydisch[7] = {0, 2, 4, 5, 7, 9, 10};
int aeolian[7] = {0, 2, 3, 5, 7, 8, 10};
int lokrisch[7] = {0, 1, 3, 5, 6, 8, 10};

struct ModeArray {
  int *data;
  String name;
  int size;
};

std::vector<ModeArray> musicModes;
ModeArray musicMode;

int randomNoteFromBase(int baseNote) {
  int idx = int(random(0, musicMode.size));
  int note = baseNote + musicMode.data[idx];
  if (note > 127){
    int note = (127 - 1) - ((127 - 1) % baseNote);
  }
  return min(baseNote + musicMode.data[idx], 127);
  // return min(baseNote + baseNote * int(random(0, 6)), 128);
}

//////////////////////////// RANDOM VOICE /////////////////////////////
// 1: playMidi+CC_Change; 2: change_cc only
void run_randomVoice() {

  static bool once = true;
  if (once){
    musicModes.push_back({dur, "dur", 3});
    musicModes.push_back({moll, "moll", 3});
    musicModes.push_back({ionisch, "ionisch", 7});
    musicModes.push_back({dorisch, "dorisch", 7});
    musicModes.push_back({phrygian, "phrygian", 7});
    musicModes.push_back({lydian, "lydian", 7});
    musicModes.push_back({mixolydisch, "mixolydisch", 7});
    musicModes.push_back({aeolian, "aeolian", 7});
    musicModes.push_back({lokrisch, "lokrisch", 7});
    once = false;
  }

  switch (Globals::active_song->step) {
  case 0: // just init

    if (Globals::active_song->isStateInit()) {

      int idx = int(random(0, musicModes.size()));
      musicMode = musicModes[idx];
      Serial.print("chosen mode is ");
      Serial.println(musicMode.name);

      // delete all midiTargets:
      for (auto &instrument : Drumset::instruments) {
        for (auto &midiTarget : instrument->midiTargets) {
          delete midiTarget;
          midiTarget = nullptr;
        }
        instrument->midiTargets.clear();
      }

      Globals::active_song
          ->resetInstruments(); // reset all instruments to "Monitor" mode
      Synthesizers::mKorg->sendProgramChange(int(random(0, 128)));
      // notes.push_back(int(random(24, 48)));
      Drumset::snare->addMidiTarget(int(random(0, 128)), Synthesizers::mKorg,
                                    115, 15, 10, -0.1);

      Drumset::kick->addMidiTarget(int(random(0, 128)), Synthesizers::mKorg,
                                   115, 15, 10, -0.1);
      Drumset::tom1->addMidiTarget(int(random(0, 128)), Synthesizers::mKorg,
                                   115, 15, 10, -0.1);
      Drumset::tom2->addMidiTarget(int(random(0, 128)), Synthesizers::mKorg,
                                   115, 15, 50, -0.1);
      Drumset::standtom->addMidiTarget(int(random(0, 128)), Synthesizers::mKorg,
                                       115, 15, 10, -0.1);
      Drumset::tom2->addMidiTarget(int(random(0, 128)), Synthesizers::mKorg,
                                   115, 15, 20, -0.06);

      Globals::active_song->increase_step(); // go to 1
    }
    break;

  case 1: // change CC ("Reflex") + PlayMidi
    if (Globals::active_song->isStateInit()) {
      Hardware::footswitch_mode = Increment_Score;

      Drumset::kick->midiTargets.back()->notes.push_back(randomNoteFromBase(Note_D3));
      Drumset::kick->midiTargets.back()->active_note =
          Drumset::kick->midiTargets.back()->notes[0];

      Drumset::snare->midiTargets.back()->notes.push_back(randomNoteFromBase(Note_D3));
      Drumset::snare->midiTargets.back()->active_note =
          Drumset::snare->midiTargets.back()->notes[0];

      Drumset::tom1->midiTargets.back()->notes.push_back(randomNoteFromBase(Note_D3));
      Drumset::tom1->midiTargets.back()->active_note =
          Drumset::tom1->midiTargets.back()->notes[0];

      Drumset::tom2->midiTargets.back()->notes.push_back(randomNoteFromBase(Note_D3));
      Drumset::tom2->midiTargets.back()->active_note =
          Drumset::tom2->midiTargets.back()->notes[0];

      Drumset::standtom->midiTargets.back()->notes.push_back(
          randomNoteFromBase(Note_D3));
      Drumset::standtom->midiTargets.back()->active_note =
          Drumset::standtom->midiTargets.back()->notes[0];
      // Drumset::standtom->addMidiTarget(mKORG_Resonance, Synthesizers::mKorg,
      // 115, 15, 10, -0.6);

      Drumset::kick->set_effect(PlayMidi);
      Drumset::snare->set_effect(PlayMidi);
      Drumset::standtom->set_effect(PlayMidi);
      Drumset::tom1->set_effect(PlayMidi);
      Drumset::tom2->set_effect(PlayMidi);
      Drumset::hihat->set_effect(TapTempo);
    }
    break;

  case 2: // change CC only
    if (Globals::active_song->isStateInit()) {
      // Hardware::footswitch_mode = Experimental;
      Drumset::kick->shuffle_cc(Drumset::kick->midiTargets.back(),
                                true); // set a random midi CC channel
      Drumset::snare->shuffle_cc(Drumset::snare->midiTargets.back(),
                                 true); // set a random midi CC channel
      Drumset::tom1->shuffle_cc(Drumset::tom1->midiTargets.back(),
                                true); // set a random midi CC channel
      Drumset::tom2->shuffle_cc(Drumset::tom2->midiTargets.back(),
                                true); // set a random midi CC channel
      Drumset::standtom->shuffle_cc(Drumset::standtom->midiTargets.back(),
                                    true); // set a random midi CC channel

      // Drumset::snare->addMidiTarget(dd200_DelayTime, Synthesizers::dd200, 89,
      // 0, -9.96, 0.08);
      Drumset::snare->set_effect(Change_CC);
      Drumset::kick->set_effect(Change_CC);
      Drumset::tom1->set_effect(Change_CC);
      Drumset::tom2->set_effect(Change_CC);
      Drumset::standtom->set_effect(Change_CC);
      Drumset::standtom->set_effect(Change_CC);
      Drumset::tom2->set_effect(Change_CC);
      Drumset::hihat->set_effect(TapTempo);

      Synthesizers::mKorg->sendControlChange(mKORG_Sustain, 127);
    }


    if (Globals::current_beat_pos == 0)
      Synthesizers::mKorg->sendNoteOn(randomNoteFromBase(Note_D1));
    break;

  default: // start over again
    Globals::active_song->step = 1;
    Globals::active_song->initState = true;
    Synthesizers::mKorg
        ->notes[Globals::active_song->notes[Globals::active_song->note_idx]] =
        false;
    // Globals::active_song->proceed_to_next_score();
    break;
  }
}