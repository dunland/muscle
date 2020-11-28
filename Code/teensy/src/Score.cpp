#include <Score.h>
#include <MIDI.h>
#include <Instruments.h>
#include <Hardware.h>

TOPOGRAPHY Score::beat_regularity; // TODO: fully implement this.
TOPOGRAPHY Score::beat_sum;
TOPOGRAPHY Score::topo_midi_effect;

int Score::step = 0;
int Score::note_idx = 0;        // points at active (bass-)note
int Score::note_change_pos = 0; // defines at what position to increase note_idx
std::vector<int> Score::notes;

boolean Score::setup = true; // when true, current score_step's setup function is executed.

void Score::add_bassNote(int note)
{
    notes.push_back(note);
    Globals::print_to_console("note ");
    Globals::print_to_console(note);
    Globals::print_to_console(" has been added to Score::notes [ ");
    for (uint8_t i = 0; i < notes.size(); i++)
    {
        Globals::print_to_console(notes[i]);
        Globals::print_to_console(" ");
    }
    Globals::println_to_console("]");
}

///////////////////////////////////////////////////////////////////////
////////////////////////////// MODES //////////////////////////////////
///////////////////////////////////////////////////////////////////////

// play note, repeatedly:
void Score::continuousBassNotes(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI, int note_change_pos_) // initiates a continuous bass note from score
{
    if (note_change_pos_ != 0)
        note_change_pos = note_change_pos_;

    // change note
    if (Globals::current_beat_pos % note_change_pos == 0) // at beginninng of each bar
        if (notes.size() > 1)
        {
            synth->sendNoteOff(notes[note_idx], MIDI);
            note_idx = (note_idx + 1) % notes.size(); // iterate through the bass notes
        }

    // play note
    if (Globals::current_beat_pos == 0)
    {
        synth->sendNoteOn(notes[note_idx], MIDI);
    }
}

// play note only once (turn on never off):
void Score::continuousBassNote(Synthesizer *synth, midi::MidiInterface<HardwareSerial> MIDI) // initiates a continuous bass note from score
{
    synth->sendNoteOn(notes[note_idx], MIDI);
}

void Score::envelope_cutoff(Synthesizer *synth, TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int cutoff_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height

    cutoff_val = max(20, cutoff_val);  // must be at least 20
    cutoff_val = min(cutoff_val, 127); // must not be greater than 127
    synth->sendControlChange(Cutoff, cutoff_val, MIDI);
}

void Score::envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth)
{
    int amp_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    // amp_val = max(0, amp_val);                                    // must be at least 0
    amp_val = min(amp_val, 127); // must not be greater than 127
    synth->sendControlChange(Amplevel, amp_val, MIDI);
}

void Score::crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI, Synthesizer *synth)
{
    int delaytime = instrument->topography.a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    delaytime = min(delaytime, 127);                                               // must not be greater than 127
    synth->sendControlChange(DelayTime, delaytime, MIDI);
}

// void Score::set_ramp(midi::MidiInterface<HardwareSerial> MIDI, CC_Type cc_type, MIDI_Instrument midi_instr, int start_value, int end_value, int duration)
// {
//     static boolean ramp_start = true;
//     static int value;
//     static unsigned long start_time;
//     static float factor;

//     if (ramp_start)
//     {
//         value = start_value;
//         start_time = millis();
//         factor = (start_value - end_value) / duration;
//         ramp_start = false;
//     }

//     MIDI.sendControlChange(cc_type, value, midi_instr);
//     value += factor;

//     if (millis() > start_time + duration)
//     {

//     }
// }
