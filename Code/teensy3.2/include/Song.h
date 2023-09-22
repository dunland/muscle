#pragma once
#include <functional>
#include <Topography.h>
#include <Devtools.h>
#include <Globals.h>
#include <vector>
// #include <Instruments.h>


class Instrument;
class Synthesizer;

class Song
{
public:
    typedef std::function<void()> Callback;
    Callback trigger_function;

    Song(Callback f, String songName) : trigger_function(f)
    {
        // setup names of elements for display on Serial Monitor:
        // beat_sum.tag = "v";
        // beat_regularity.tag = "r";

        // add a bass note to Score
        notes.push_back(int(random(36, 48)));
        Devtools::print_to_console("active_score->note[0] = ");
        Devtools::println_to_console(notes[0]);
        name = songName;
        setup_state = true;
    }

    String name; // name to be displayed on LCD

    int step = 0;
    std::vector<int> steps;
    int note_idx = 0;    // points at active (bass-)note
    int note_change_pos; // defines at what position to increase note_idx
    std::vector<int> notes;

    struct tempo
    {
        int min_tempo = 0;
        int max_tempo = 999;
        int tapTempoResetTime = 0;           // time to restart tapTempo if not used for this long
        unsigned int tapTempoTimeOut = 2000; // do not count second tap, if time gap to first one exceeds this
    } tempo;

    bool setup_state; // when true, current score_step's setup function is executed.
    bool get_setup_state();

    // TODO: move this to Rhythmics
    TOPOGRAPHY beat_sum;         // sum of all instrument topographies
    TOPOGRAPHY beat_regularity;  // for advance of step
    TOPOGRAPHY topo_midi_effect; // for TopographyMidiEffect

    // --------------------------- SETUP etc: -------------------------
    void set_notes(std::vector<int> list);
    void add_bassNote(int note); // adds a NOTE to notes[]
    void increase_step();
    void proceed_to_next_score();
    void setTempoRange(int min_tempo_, int max_tempo_);
    void resetInstruments();

    // ------------------------------- MODES: (deprecated) ------------
    void playRhythmicNotes(Synthesizer *synth,int note_change_pos_ = 0); // initiates a continuous bass note from score

    // void playSingleNote(Synthesizer *synth, ); // play note only once (turn on, never off)

    void playLastThreeNotes(Synthesizer *synth);

    void envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography); // creates an envelope for cutoff filter via topography

    void envelope_volume(TOPOGRAPHY *topography, Synthesizer *synth); // creates an envelope for volume filter via topography

    void crazyDelays(Instrument *instrument, Synthesizer *synth); // changes the delay times on each 16th-step

    // void set_ramp(CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration);
};

void run_monitoring();

void run_sattelstein();

void run_doubleSquirrel(); // TODO: tentative, as this should be dynamic later..

void run_host();

void run_randomVoice();

void run_A_72();

void run_control_dd200();

void run_dd200_timeControl();

void run_whammyMountains();

void run_hutschnur();

void run_control_volca();

void run_visuals();

void run_besen();

void run_nanokontrol();

void run_PogoNumberOne();

void run_roeskur();

void run_b_27();

void run_b_36();

void run_alhambra();

void run_theodolit();

void run_kupferUndGold();

void run_ferdinandPiech();

void run_donnerwetter();

void run_randomSelect();

void run_wueste();

void run_intro();

// ------------------------------- MODES: (deprecated) ------------
void playRhythmicNotes(Synthesizer *synth, int note_change_pos_ = 0); // initiates a continuous bass note from score

// void playSingleNote(Synthesizer *synth, ); // play note only once (turn on, never off)

void playLastThreeNotes(Synthesizer *synths);

void envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography); // creates an envelope for cutoff filter via topography

void envelope_volume(TOPOGRAPHY *topography, Synthesizer *synth); // creates an envelope for volume filter via topography

void crazyDelays(Instrument *instrument, Synthesizer *synth); // changes the delay times on each 16th-step

// void set_ramp( , CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration);

void run_A_15();

void run_A_25();

void run_b_11();

void run_b_36();

void run_b_63();

void run_b_73();
