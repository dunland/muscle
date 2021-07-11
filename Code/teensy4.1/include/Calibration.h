#pragma once

#include <Instruments.h>

enum Calibration_Mode
{
    Select_Instrument,
    Select_Sensitivity_Param,
    Set_Value
};

class Calibration
{

public:

static Instrument *selected_instrument;

static Calibration_Mode calibration_mode;

// static Calibration_Sensitivity_Param selected_sensitivity_param;
static int selected_sensitivity_param;
static const int SET_THRESHOLD = 0;
static const int SET_CROSSINGS = 1;
static const int SET_DELAY_AFTER_STROKE = 2;
static const int SET_STROKE_DETECTION_AFTER_FIRST = 3;

static int recent_threshold_crossings; // rememers amount of crossings of last detected stroke

static void setup();

static void set(int val);

static void update(); // check rotary encoder to scroll menus

};