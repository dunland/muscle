#include <Globals.h>
#include <Devtools.h>
// #include <Tsunami.h>
#include <ArduinoJson.h>

IntervalTimer Globals::masterClock; // 1 bar
MachineState Globals::machine_state = Running;

// Tsunami Globals::tsunami;

// ------------------------------- Score ----------------------------
std::vector<Song *> Globals::songlist; // all relevant scores
int Globals::active_song_pointer = 0;	  // points at active score of scores_list
Song *Globals::active_song;

// Tusnami Sample list:
// float Globals::track_bpm[256] =
// 	{
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
// 		1, 1, 90, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 73, 1, 1, 1,
// 		100, 1, 1, 1, 200, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 41, 1, 1,
// 		103, 1, 1, 1, 1, 1, 1, 93, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 100, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
// 		1, 1, 78, 1, 100, 100, 1, 1, 1, 1,
// 		1, 1, 1, 1, 1, 100, 1, 1, 1, 1,
// 		1, 1, 1, 1, 100, 60};

// int swell_val[numInputs]; // this should be done in the swell section, but is needed in print section already... :/

int Globals::current_beat_pos = 0;	   // always stores the current position in the beat
int Globals::current_eighth_count = 0; // overflows at current_beat_pos % 8
int Globals::current_16th_count = 0;   // overflows at current_beat_pos % 2
int Globals::last_eighth_count = 0;	   // stores last eightNoteCount for comparison
int Globals::last_16th_count = 0;	   // stores last eightNoteCount for comparison

int Globals::tapInterval = 500; // 0.5 s per beat for 120 BPM
int Globals::current_BPM = 120;
bool Globals::bSendMidiClock = true;
bool Globals::bUsingSDCard = false;

// ------------------------------- Hardware -----------------------------------
boolean Globals::footswitch_is_pressed = false;

// ----------------------------- timer counter ---------------------------------
volatile unsigned long Globals::masterClockCount = 0; // 4*32 = 128 masterClockCount per cycle
volatile unsigned long Globals::beatCount = 0;
int Globals::next_beatCount = 0; // will be reset when timer restarts
volatile boolean Globals::sendMidiClock = false;

void Globals::masterClockTimer()
{
	/*
    timing              fraction          bar @ 120 BPM    bar @ 180 BPM
    1 bar             = 1               = 2 s       | 1.3 s
    1 beatCount       = 1/32 bar        = 62.5 ms   | 41.7 ms
    stroke precision  = 1/4 beatCount   = 15.625 ms | 10.4166 ms


    |     .-.
    |    /   \         .-.
    |   /     \       /   \       .-.     .-.     _   _
    +--/-------\-----/-----\-----/---\---/---\---/-\-/-\/\/---
    | /         \   /       \   /     '-'     '-'
    |/           '-'         '-'
    |--------2 oscil---------|
    |------snare = 10 ms-----|

  */

	masterClockCount++; // will rise infinitely

	// ------------ 1/32 increase in 4 full precisionCounts -----------
	if (masterClockCount % 4 == 0)
	{
		beatCount++; // will rise infinitely
	}

	// evaluate current position of beat in bar for stroke precision
	// 2020-09-07: this doesn't help and it's also not working...
	// if ((masterClockCount % 4) >= next_beatCount - 2)
	// {
	//   currentStep = next_beatCount;
	//   next_beatCount += 4;
	// }

	// prepare MIDI clock:
	sendMidiClock = (((masterClockCount % 4) / 3) % 4 == 0);
}
// ---------------------------------------------------------------------------


// ----------------------------- Auxiliary --------------------------

String Globals::DrumtypeToHumanreadable(DrumType type)
{
	switch (type)
	{
	case Snare:
		return "Snare";
	case Hihat:
		return "Hihat";
	case Kick:
		return "Kick";
	case Tom1:
		return "Tom1";
	case Tom2:
		return "Tom2";
	case Standtom1:
		return "S_Tom1";
	case Standtom2:
		return "S_Tom2";
	case Ride:
		return "Ride";
	case Crash1:
		return "Crash1";
	case Crash2:
		return "Crash2";
	case Cowbell:
		return "Cowbell";
	}
	return "";
}

String Globals::EffectstypeToHumanReadable(EffectsType type)
{
	switch (type)
	{
	case PlayMidi:
		return "PlayMidi";
	case Monitor:
		return "Monitor";
	case ToggleRhythmSlot:
		return "ToggleRhythmSlot";
	case FootSwitchLooper:
		return "FootSwitchLooper";
	case TapTempo:
		return "TapTempo";
	case TapTempoRange:
		return "TapTempoRange";
	case Swell:
		return "Swell";
	case TsunamiLink:
		return "TsunamiLink";
	case CymbalSwell:
		return "CymbalSwell";
	case TopographyMidiEffect:
		return "TopographyMidiEffect";
	case Change_CC:
		return "Change_CC";
	case Random_CC_Effect:
		return "Random_CC_Effect";
	case MainNoteIteration:
		return "MainNoteIteration";
	case Reflex_and_PlayMidi:
		return "Reflex_and_PlayMidi";
	}
	return "";
}

// TODO: different enums for each device, so they don't overlap?
String Globals::CCTypeToHumanReadable(CC_Type type)
{
	switch (type)
	{
		case CC_None: 					return "None"; 				break;
		case dd200_DelayTime: 		return "dTim"; 				break;
		case mKORG_Osc2_semitone: 	return "O2smi"; 			break;
		case dd200_param:			return "dprm";				break;
		// case Osc2_tune: 			return "Osc2_tune"; 		break;
		case dd200_DelayLevel: 		return "dLvl";			 	break;
		case mKORG_Mix_Level_1: 	return "Mix1"; 				break;
		case mKORG_Mix_Level_2: 	return "Mix2";		 		break;
		case dd200_OnOff: 			return "OnOf";		 		break;
		case mKORG_Patch_1_Depth: 	return "P1D"; 				break;
		case mKORG_Patch_3_Depth: 	return "P3D"; 				break;
		case mKORG_Cutoff: 			return "Ctf"; 				break;
		case mKORG_LFO1_Rate: 		return "LF1r"; 				break;
		case mKORG_LFO2_Rate: 		return "LF2r"; 				break;
		case mKORG_Resonance: 		return "Reso";		 		break;
		case mKORG_Amplevel: 		return "Amp"; 				break;
		case mKORG_Attack: 			return "atk"; 				break;
		case mKORG_Sustain: 		return "sus"; 				break;
		case mKORG_Release: 		return "Rls"; 				break;
		case mKORG_DelayTime: 		return "dTim"; 				break;
		// case DelayDepth: 		return "DelayDepth"; 		break;
		case dd200_DelayDepth: 		return "dDpt";			 	break;
		case mKORG_TimbreSelect: 	return "Tmbr"; 				break;
		default: 					return "n.d.";		 		break;
	}
}

CC_Type Globals::int_to_cc_type(int integer) // TODO: can this not be done with casting?
{
	switch (integer)
	{
	case 18:
		return mKORG_Osc2_semitone;
	case 19:
		return mKORG_Osc2_tune;
	case 20:
		return mKORG_Mix_Level_1;
	case 21:
		return mKORG_Mix_Level_2;
	case 28:
		return mKORG_Patch_1_Depth;
	case 30:
		return mKORG_Patch_3_Depth;
	case 44:
		return mKORG_Cutoff;
	case 46:
		return mKORG_LFO1_Rate;
	case 76:
		return mKORG_LFO2_Rate;
	case 71:
		return mKORG_Resonance;
	case 50:
		return mKORG_Amplevel;
	case 23:
		return mKORG_Attack;
	case 25:
		return mKORG_Sustain;
	case 26:
		return mKORG_Release;
	case 51:
		return mKORG_DelayTime;
	case 94:
		return mKORG_DelayDepth;

	// ATTENTION:
	// IMPORTANT:
	// TODO: define all CC channels, otherwise this procedure can take forever in the while loop!
	default:
		Devtools::print_to_console("could not convert integer ");
		Devtools::print_to_console(integer);
		Devtools::println_to_console("to CC_Type. Reset to None.");
		return CC_None;
	}
}