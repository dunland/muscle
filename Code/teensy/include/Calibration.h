#pragma once

#include <Instruments.h>

enum Feature_Type
{
    Select_Instrument,
    Select_Value,
    Set_Threshold,
    Set_Crossings,
    Set_Delay_After_Stroke,
    Set_Stroke_Detection_After_First
};

class Calibration
{

public:

static Instrument *selected_instrument;

static Feature_Type current_feature;

static void set_value(int val);

};