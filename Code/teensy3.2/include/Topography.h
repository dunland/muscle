# pragma once
#include <Arduino.h>
#include <vector>
#include <Globals.h>
#include <Devtools.h>

class TOPOGRAPHY
{
public:
  String tag; // very short name for topography. also to be sent via Serial to processing

  std::vector<int> a_8 = {0, 0, 0, 0, 0, 0, 0, 0};                          // size-8 array for comparison with 8-bit-length sound files
  std::vector<int> a_16 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // size-16 array for abstractions like beat regularity etc
  std::vector<int> a_16_prior = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  int snr_thresh = 3;         // threshold for signal-to-noise-ratio to be smoothened
  int activation_thresh = 10; // threshold in average_smooth to activate next action
  int average_smooth = 0;
  int regular_sum = 0;

  bool ready(); // holds whether average_smooth has reached activation_thresh

  boolean flag_entry_dismissed = false; // indicates that an entry has been dropped due to too high topography difference

  // -------------------------- regularity ----------------------------
  int regularity = 0; // the regularity of a played instrument boiled down to one value

  bool flag_empty_increased[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};    // indicates that an empty slot has repeatedly NOT been played → increase
  bool flag_occupied_increased[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false}; // indicates that an occupied slot has repeatedly been played → increase
  bool flag_empty_played[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};       // indicates that an empty slot WAS played → decrease
  bool flag_occupied_missed[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};    // indicates that an occupied slot has NOT been played → decrease

  // ---------------------------- functions ---------------------------
  void reset();
  void add(TOPOGRAPHY *to_add);
  void smoothen_dataArray(); // there is a double of this in Instruments to perform instrument-specific operations
  void derive_from(TOPOGRAPHY *original);
  void print();
};