#pragma once
#include <functional>

#include <Globals.h>
#include <vector>
// #include <Instruments.h>
#include <MIDI.h>

class Instrument;
class Synthesizer;

class Song
{
public:

    typedef std::function<void(midi::MidiInterface<HardwareSerial>)> Callback;
    Callback trigger_function;

    Song(Callback f) : trigger_function(f)
    {
        // setup names of elements for display on Serial Monitor:
        beat_sum.tag = "v";
        beat_regularity.tag = "r";

        // add a bass note to Score
        notes.push_back(int(random(36, 48)));
        Globals::print_to_console("active_score->note[0] = ");
        Globals::println_to_console(notes[0]);
    }

    String name; // name to be displayed on LCD

    static int step;
    std::vector<int> steps;
    static int note_idx;        // points at active (bass-)note
    static int note_change_pos; // defines at what position to increase note_idx
    static std::vector<int> notes;

    struct tempo
    {
        int min_tempo = 0;
        int max_tempo = 999;
        int tapTempoResetTime = 0;           // time to restart tapTempo if not used for this long
        unsigned int tapTempoTimeOut = 2000; // do not count second tap, if time gap to first one exceeds this
    };

    static tempo tempo;

    // TODO: make this a functional bool and reset instruments etc when calling (and deactivate it automatically)!
    static bool setup; // when true, current score_step's setup function is executed.

    // TODO: move this to Rhythmics
    static TOPOGRAPHY beat_sum;         // sum of all instrument topographies
    static TOPOGRAPHY beat_regularity;  // for advance of step
    static TOPOGRAPHY topo_midi_effect; // for TopographyMidiEffect

    // --------------------------- SETUP etc: -------------------------
    static void set_step_function(int trigger_step, Instrument *instrument, EffectsType); // TODO: set score-step-functions here
    static void set_notes(std::vector<int> list);
    static void add_bassNote(int note); // adds a NOTE to notes[]
    static void increase_step();
    static void proceed_to_next_score();
    static void setTempoRange(int min_tempo_, int max_tempo_);
    static void resetInstruments();

    // ---------------------------- SONGS: ---------------------------
    // STANDARD RUN: select according to score->name
    // void run(midi::MidiInterface<HardwareSerial> MIDI); // iterates through all score steps, executing the current step functions
    // ------------------------------- MODES: (deprecated) ------------
    static void playRhythmicNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI, int note_change_pos_ = 0); // initiates a continuous bass note from score

    static void playSingleNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI); // play note only once (turn on, never off)

    static void playLastThreeNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI);

    static void envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI); // creates an envelope for cutoff filter via topography

    static void envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // creates an envelope for volume filter via topography

    static void crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth); // changes the delay times on each 16th-step

    // void set_ramp( midi::MidiInterface<HardwareSerial> MIDI, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration);
};

    void run_monitoring(midi::MidiInterface<HardwareSerial> MIDI);

    void run_sattelstein(midi::MidiInterface<HardwareSerial> MIDI);

    void run_doubleSquirrel(midi::MidiInterface<HardwareSerial> MIDI); // TODO: tentative, as this should be dynamic later..

    void run_elektrosmoff(midi::MidiInterface<HardwareSerial> MIDI);

    void run_randomVoice(midi::MidiInterface<HardwareSerial> MIDI);

    void run_a72(midi::MidiInterface<HardwareSerial> MIDI);

    void run_control_dd200(midi::MidiInterface<HardwareSerial> MIDI);

    void run_dd200_timeControl(midi::MidiInterface<HardwareSerial> MIDI);

    void run_whammyMountains(midi::MidiInterface<HardwareSerial> MIDI);

    void run_hutschnur(midi::MidiInterface<HardwareSerial> MIDI);

    void run_control_volca(midi::MidiInterface<HardwareSerial> MIDI);

    void run_visuals(midi::MidiInterface<HardwareSerial> MIDI);

    void run_zitteraal(midi::MidiInterface<HardwareSerial> MIDI);

    void run_nanokontrol(midi::MidiInterface<HardwareSerial> MIDI);

    void run_PogoNumberOne(midi::MidiInterface<HardwareSerial> MIDI);

    void run_roeskur(midi::MidiInterface<HardwareSerial> MIDI);

    void run_b_27(midi::MidiInterface<HardwareSerial> MIDI);

    void run_b_36(midi::MidiInterface<HardwareSerial> MIDI);

    void run_alhambra(midi::MidiInterface<HardwareSerial> MIDI);

    void run_theodolit(midi::MidiInterface<HardwareSerial> MIDI);

    void run_kupferUndGold(midi::MidiInterface<HardwareSerial> MIDI);

    void run_donnerwetter(midi::MidiInterface<HardwareSerial> MIDI);
