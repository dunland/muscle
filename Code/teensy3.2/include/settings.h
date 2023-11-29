// SETTINGS
// PINOUT, MIDI CHANNELS AND SENSITIVITY
#pragma once

////////////////////////////// MIDI CHANNELS //////////////////////////
#define MIDI_CHANNEL_VOLCA 1
#define MIDI_CHANNEL_MICROKORG 2
#define MIDI_CHANNEL_DD200 3
#define MIDI_CHANNEL_WHAMMY 4
#define MIDI_CHANNEL_KP3 5

/////////////////////////// AUXILIARY HARDWARE ////////////////////////
#define VIBR 0
#define FOOTSWITCH 2

/* LCD AV1623 pinout:
1   Vss       GND
2   Vdd       +5V
3   V0 (LCD Drive poti power) / GND
4   RS        11
5   R/W       GND
6   EN        12
7   D0        --
8   D1        --
9   D2        --
10  D3        --
11  D4        10    // 8 on teensy3.2
12  D5        9
13  D6        4
14  D7        5
15  A         +5V
16  K         GND
*/

// LCD Pins:
#define RS 11
#define EN 12
#define D4 10 // 8 on teensy3.2
#define D5 9
#define D6 4
#define D7 5

// Encoder Pins:
// right - encoder1 (3)
// middle - ground (GND)
// left - encoder2 (6)
#define ENCODER1 6
#define ENCODER2 3
#define PUSHBUTTON 28 // 7 on teensy3.2

//////////////////////////////// PIEZO INPUTS /////////////////////////

// ATTENTION: PCB-Layout ist falsch!!
// PCB-pin 38 == teensy-pin 16 == A2 statt A0!!

#define INPUT_PIN_CRASH1 A9
#define INPUT_PIN_KICK A3
#define INPUT_PIN_STANDTOM A4
#define INPUT_PIN_RIDE A8
#define INPUT_PIN_SNARE A2
#define INPUT_PIN_HIHAT A6
#define INPUT_PIN_TOM1 A7
#define INPUT_PIN_TOM2 A5

#define SNARE_THRESHOLD 40
#define SNARE_CROSSINGS 3
#define SNARE_DELAY_AFTER_STROKE 10
#define SNARE_FIRST_STROKE true

#define HIHAT_THRESHOLD 100
#define HIHAT_CROSSINGS 10
#define HIHAT_DELAY_AFTER_STROKE 5
#define HIHAT_FIRST_STROKE false

#define KICK_THRESHOLD 80
#define KICK_CROSSINGS 5
#define KICK_DELAY_AFTER_STROKE 20
#define KICK_FIRST_STROKE true

#define TOM1_THRESHOLD 65
#define TOM1_CROSSINGS 4
#define TOM1_DELAY_AFTER_STROKE 5
#define TOM1_FIRST_STROKE false

#define TOM2_THRESHOLD 50
#define TOM2_CROSSINGS 4
#define TOM2_DELAY_AFTER_STROKE 5
#define TOM2_FIRST_STROKE false

#define STANDTOM_THRESHOLD 65
#define STANDTOM_CROSSINGS 4
#define STANDTOM_DELAY_AFTER_STROKE 7
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

//////////////////////////////// CORE /////////////////////////////////

#define STARTUP_TIME_WAIT_FOR_USB 5000
#define ENCODER_TIMEOUT 100