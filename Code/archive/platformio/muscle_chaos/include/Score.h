// ----------------- MUSICAL AND PERFORMATIVE PARAMETERS --------------
#ifndef SCORE_H
#define SCORE_H

#include <Arduino.h>
#include <Globals.h>

class Score
{
public:
  boolean read_rhythm_slot[Globals::numInputs][8];
  boolean set_rhythm_slot[Globals::numInputs][8];
  // int beat_topography_8[Globals::numInputs][8]; // TODO: use structs instead!
  // int beat_topography_16[Globals::numInputs][16];

  int notes_list[];
  int cc_chan[]; // needed in pinAction 5 and 6

  int initialPinAction[Globals::numInputs]; // holds the pinAction array as defined above

  int allocated_track[Globals::numInputs]; // tracks will be allocated in tsunami_beat_playback
  int allocated_channels[];                // channels to send audio from tsunami to

  float track_bpm[];
};

#endif