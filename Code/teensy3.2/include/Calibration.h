#pragma once

#include <Instruments.h>

class Calibration
{

static Instrument *selected_instrument;
static int instr_locator;

static void select_instrument(Instrument *selected_instrument, int value);

static void set_threshold(Instrument *selected_instrument, int value);

static void set_crossings(Instrument *selected_instrument, int value);

static void set_delay_after_stroke(Instrument *selected_instrument, int value);

static void set_stroke_detection_after_first(Instrument *selected_instrument, boolean);

};