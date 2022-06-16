// SETTINGS
#pragma once

////////////////////////////// MIDI CHANNELS //////////////////////////
#define MIDI_CHANNEL_VOLCA 1
#define MIDI_CHANNEL_MICROKORG 2
#define MIDI_CHANNEL_DD200 3
#define MIDI_CHANNEL_WHAMMY 4

/////////////////////////// AUXILIARY HARDWARE ////////////////////////
#define VIBR 0
#define FOOTSWITCH 2

/* LCD pinout:
1   Vss       GND
2   Vss       +5V
3   LCD Drive poti power / GND
4   RS        11
5   R/W       GND
6   EN        12
7   D0        --
8   D1        --
9   D2        --
10  D3        --
11  D4        8
12  D5        9
13  D6        4
14  D7        5
15  A         +5V
16  K         GND
*/

// LCD Pins:
#define RS 11
#define EN 12
#define D4 8
#define D5 9
#define D6 4
#define D7 5

// Encoder Pins:
// right - encoder1
// middle - ground
// left - encoder2
#define ENCODER1 3
#define ENCODER2 6
#define PUSHBUTTON 7

//////////////////////////////// PIEZO INPUTS /////////////////////////
#define INPUT_PIN_CRASH1 A5
#define INPUT_PIN_KICK A1
#define INPUT_PIN_STANDTOM A2
#define INPUT_PIN_RIDE A4
#define INPUT_PIN_SNARE A0
#define INPUT_PIN_HIHAT A6
#define INPUT_PIN_TOM1 A7
#define INPUT_PIN_TOM2 A3

#define SNARE_THRESHOLD 115
#define SNARE_CROSSINGS 5
#define SNARE_DELAY_AFTER_STROKE 5
#define SNARE_FIRST_STROKE true

#define HIHAT_THRESHOLD 100
#define HIHAT_CROSSINGS 10
#define HIHAT_DELAY_AFTER_STROKE 5
#define HIHAT_FIRST_STROKE false

#define KICK_THRESHOLD 200
#define KICK_CROSSINGS 5
#define KICK_DELAY_AFTER_STROKE 15
#define KICK_FIRST_STROKE true

#define TOM1_THRESHOLD 200
#define TOM1_CROSSINGS 20
#define TOM1_DELAY_AFTER_STROKE 10
#define TOM1_FIRST_STROKE false

#define TOM2_THRESHOLD 100
#define TOM2_CROSSINGS 9
#define TOM2_DELAY_AFTER_STROKE 10
#define TOM2_FIRST_STROKE false

#define STANDTOM_THRESHOLD 200
#define STANDTOM_CROSSINGS 10
#define STANDTOM_DELAY_AFTER_STROKE 10
#define STANDTOM_FIRST_STROKE false

#define CRASH1_THRESHOLD 170
#define CRASH1_CROSSINGS 2
#define CRASH1_DELAY_AFTER_STROKE 5
#define CRASH1_FIRST_STROKE true

#define CRASH2_THRESHOLD
#define CRASH2_CROSSINGS
#define CRASH2_DELAY_AFTER_STROKE
#define CRASH2_FIRST_STROKE

#define RIDE_THRESHOLD 300
#define RIDE_CROSSINGS 2
#define RIDE_DELAY_AFTER_STROKE 5
#define RIDE_FIRST_STROKE true

#define COWBELL_THRESHOLD 80
#define COWBELL_CROSSINGS 15
#define COWBELL_DELAY_AFTER_STROKE 10
#define COWBELL_FIRST_STROKE false

