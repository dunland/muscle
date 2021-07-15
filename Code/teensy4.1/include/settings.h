// SETTINGS

#pragma once

/////////////////////////////// KNOBS /////////////////////////////////

#define VIBRATION_MOTOR_PIN 0
#define FOOTSWITCH_PIN 2
#define ENCODER1 3
#define ENCODER2 6
#define PUSHBUTTON 40

// Encoder Pins:
// right - encoder1
// middle - ground
// left - encoder2

// Pushbutton:
// left - 7
// right - GND

///////////////////////////// INSTRUMENTS /////////////////////////////

#define SNARE_THRESHOLD 70
#define SNARE_CROSSINGS 10
#define SNARE_DELAY_AFTER_STROKE 10
#define SNARE_FIRST_STROKE false

// #define SNARE_THRESHOLD 2
// #define SNARE_CROSSINGS 2
// #define SNARE_DELAY_AFTER_STROKE 10
// #define SNARE_FIRST_STROKE true

#define HIHAT_THRESHOLD 80 
#define HIHAT_CROSSINGS 15
#define HIHAT_DELAY_AFTER_STROKE 10
#define HIHAT_FIRST_STROKE false

#define KICK_THRESHOLD 110
#define KICK_CROSSINGS 12
#define KICK_DELAY_AFTER_STROKE 10
#define KICK_FIRST_STROKE false

#define TOM1_THRESHOLD 200
#define TOM1_CROSSINGS 20
#define TOM1_DELAY_AFTER_STROKE 10
#define TOM1_FIRST_STROKE false

#define TOM2_THRESHOLD 100
#define TOM2_CROSSINGS 9
#define TOM2_DELAY_AFTER_STROKE 10
#define TOM2_FIRST_STROKE false

#define STANDTOM_THRESHOLD 140
#define STANDTOM_CROSSINGS 10
#define STANDTOM_DELAY_AFTER_STROKE 10
#define STANDTOM_FIRST_STROKE false

#define CRASH1_THRESHOLD 300
#define CRASH1_CROSSINGS 2
#define CRASH1_DELAY_AFTER_STROKE 5
#define CRASH1_FIRST_STROKE true

#define CRASH2_THRESHOLD
#define CRASH2_CROSSINGS
#define CRASH2_DELAY_AFTER_STROKE
#define CRASH2_FIRST_STROKE

#define RIDE_THRESHOLD 400
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