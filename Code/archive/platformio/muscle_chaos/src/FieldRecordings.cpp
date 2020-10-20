#include <Globals.h>
#include <Arduino.h>
#include <FieldRecordings.h>

/* ---------- Tsunami Beat-linked Playback algorithm: -----------------


  X-----------X---X--------------- 32nd Beat
  [1,  0,  0,  1,  1,  0,  0,  0  ] 1. iteration
  |-----------|---|---------------
  +1, +0, +0, +1, +1, +0, +0, +0    changes between iterations
  |-----------|---|---------------
  [2,  0,  0,  2,  2,  0,  0,  0  ] 2. iteration
  |-----------|---|---------------
  +0. +1, +0, +1, +1, +0, +0, +0    changes, unprecisely played
  ----|-------|---|---------------
  [2,  1,  0,  3,  3,  0,  0,  0  ] 3. iteration
  |-----------|---|---------------
  4 + 1 + 0 + 9 + 9 + 0 + 0 + 0 = 23 beat_topo_squared_sum
  |-----------|---|---------------

  4/23 = 0.174
          ratio = 4:1 = 0.25
  1/23 = 0.043
          ratio = 9:1 = 0.111 --> beat_topo_entries -= 1; beat_topo[i] = 0
  9/23 = 0.391

  beat_topo_regular_sum = 2 + 1 + 3 + 3 = 9
  smoothing: beat_topo_entries = 4 - 1 = 3
  beat_topo_average_smooth = int(9/3 + 0.5) = 3

  ---------------------------------------------------------------------*/

void FieldRecordings::tsunami_beat_playback(int instr, int current_beat_in)
{
    // ------------------ create topography and smoothen it -------------
    // smoothen_dataArray(Score::beat_topography_8, instr, 3)
    int beat_topo_entries = 0;
    int beat_topo_squared_sum = 0;
    int beat_topo_regular_sum = 0;

    // count entries and create squared sum:
    for (int j = 0; j < 8; j++)
    {
        if (Score::beat_topography_8.array[instr][j] > 0)
        {
            beat_topo_entries++;
            beat_topo_squared_sum += Score::beat_topography_8.array[instr][j] * Score::beat_topography_8.array[instr][j];
            beat_topo_regular_sum += Score::beat_topography_8.array[instr][j];
        }
    }

    beat_topo_regular_sum = beat_topo_regular_sum / beat_topo_entries;

    // calculate site-specific fractions (squared):
    float beat_topo_squared_frac[8];
    for (int j = 0; j < 8; j++)
        beat_topo_squared_frac[j] =
            float(Score::beat_topography_8.array[instr][j]) / float(beat_topo_squared_sum);

    // get highest frac:
    float highest_frac = 0;
    for (int j = 0; j < 8; j++)
        highest_frac = (beat_topo_squared_frac[j] > highest_frac) ? beat_topo_squared_frac[j] : highest_frac;

    // get "topography height":
    // divide highest with other entries and omit entries if ratio > 3:
    for (int j = 0; j < 8; j++)
        if (beat_topo_squared_frac[j] > 0)
            if (highest_frac / beat_topo_squared_frac[j] > 3 || beat_topo_squared_frac[j] / highest_frac > 3)
            {
                Score::beat_topography_8.array[instr][j] = 0;
                beat_topo_entries -= 1;
                // Serial.print(DrumtypeToHumanreadable((DrumType)j));
                Serial.print(": REDUCED VAL AT POS ");
                Serial.println(j);
            }

    int beat_topo_average_smooth = 0;
    // assess average topo sum for loudness
    for (int j = 0; j < 8; j++)
        beat_topo_regular_sum += Score::beat_topography_8.array[instr][j];
    beat_topo_average_smooth = int((float(beat_topo_regular_sum) / float(beat_topo_entries)) + 0.5);

    // TODO: reduce all params if not played for long.

    int tracknum = 0;               // Debug
    if (beat_topo_regular_sum >= 3) // only initiate playback if average of entries > certain threshold.
    {

        // find right track from database:
        //int tracknum = 0;
        for (int j = 0; j < 8; j++)
        {
            if (Score::beat_topography_8.array[instr][j] > 0)
            {
                if (j == 0)
                    tracknum += 128;
                if (j == 1)
                    tracknum += 64;
                if (j == 2)
                    tracknum += 32;
                if (j == 3)
                    tracknum += 16;
                if (j == 4)
                    tracknum += 8;
                if (j == 5)
                    tracknum += 4;
                if (j == 6)
                    tracknum += 2;
                if (j == 7)
                    tracknum += 1;
            }
        }
        Score::allocated_track[instr] = tracknum; // save for use in other functions

        // set loudness and fade:
        //int trackLevel = min(-40 + (beat_topo_average_smooth * 5), 0);
        int trackLevel = 0;                                                            // Debug
        Hardware::tsunami.trackFade(tracknum, trackLevel, Timing::tapInterval, false); // fade smoothly within length of a quarter note

        // TODO: set track channels for each instrument according to output
        // output A: speaker on drumset
        // output B: speaker in room (PA)
        // cool effects: let sounds walk through room from drumset to PA

        // --------------------------- play track -------------------------
        if (!Hardware::tsunami.isTrackPlaying(tracknum) && current_beat_in == 0)
        {
            // set playback speed according to current_BPM:
            int sr_offset;
            float r = Timing::current_BPM / float(Score::track_bpm[tracknum]);
            Serial.print("r = ");
            Serial.println(r);
            if (!(r > 2) && !(r < 0.5))
            {
                // samplerateOffset scales playback speeds from 0.5 to 1 to 2
                // and maps to -32768 to 0 to 32767
                sr_offset = (r >= 1) ? 32767 * (r - 1) : -32768 + 32768 * 2 * (r - 0.5);
                sr_offset = int(sr_offset);
                Serial.print("sr_offset = ");
                Serial.println(sr_offset);
            }
            else
            {
                sr_offset = 0;
            }

            //int channel = 0;                              // Debug
            Hardware::tsunami.samplerateOffset(Score::allocated_channels[instr], sr_offset); // TODO: link channels to instruments
            Hardware::tsunami.trackGain(tracknum, trackLevel);
            Hardware::tsunami.trackPlayPoly(tracknum, Score::allocated_channels[instr], true); // If TRUE, the track will not be subject to Tsunami's voice stealing algorithm.
            Serial.print("starting to play track ");
            Serial.println(tracknum);
        } // track playing end
    }     // threshold end

    // Debug:
    Serial.print("[");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(Score::beat_topography_8.array[instr][i]);
        if (i < 7)
            Serial.print(", ");
    }
    Serial.print("]\t");
    //  Serial.print(beat_topo_entries);
    //  Serial.print("\t");
    //  Serial.print(beat_topo_squared_sum);
    //  Serial.print("\t");
    //  Serial.print(beat_topo_regular_sum);
    //  Serial.print("\t");
    //  Serial.print(beat_topo_average_smooth);
    Serial.print("\t");
    int trackLevel = min(-40 + (beat_topo_average_smooth * 5), 0);
    Serial.print(trackLevel);
    Serial.print("dB\t->");
    Serial.println(tracknum);
}
// --------------------------------------------------------------------