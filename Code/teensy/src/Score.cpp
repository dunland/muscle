#include <Score.h>
#include <MIDI.h>
#include <Instruments.h>

// TODO:
//TOPOGRAPHY Score::beat_regularity;

TOPOGRAPHY Score::beat_sum;

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
    for (int i = 0; i < notes.size(); i++)
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
void Score::continuousBassNotes(midi::MidiInterface<HardwareSerial> MIDI, MIDI_Instrument midi_instrument) // initiates a continuous bass note from score
{
    // change note
    if (Globals::current_beat_pos % note_change_pos == 0) // at beginninng of each bar
        if (notes.size() > 1)
        {
            MIDI.sendNoteOff(notes[note_idx], 127, midi_instrument);
            note_idx = (note_idx + 1) % notes.size(); // iterate through the bass notes
        }

    // play note
    if (Globals::current_beat_pos == 0)
    {
        MIDI.sendNoteOn(notes[note_idx], 127, midi_instrument);
    }
}

// play note only once (turn on never off):
void Score::continuousBassNote(midi::MidiInterface<HardwareSerial> MIDI) // initiates a continuous bass note from score
{
    MIDI.sendNoteOn(notes[0], 127, microKORG);
}

void Score::envelope_cutoff(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int cutoff_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    cutoff_val = max(20, cutoff_val);                                    // must be at least 20
    cutoff_val = min(cutoff_val, 127);                                   // must not be greater than 127
    MIDI.sendControlChange(44, cutoff_val, 2);
}

void Score::envelope_volume(TOPOGRAPHY *topography, midi::MidiInterface<HardwareSerial> MIDI)
{
    int amp_val = topography->a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    // amp_val = max(0, amp_val);                                    // must be at least 0
    amp_val = min(amp_val, 127);            // must not be greater than 127
    MIDI.sendControlChange(50, amp_val, 2); // set loudness
}

void Score::crazyDelays(Instrument *instrument, midi::MidiInterface<HardwareSerial> MIDI)
{
    int delaytime = instrument->topography.a_16[Globals::current_16th_count] * 13; // create cutoff value as a factor of topography height
    delaytime = min(delaytime, 127);                                               // must not be greater than 127
    MIDI.sendControlChange(51, delaytime, 2);
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
