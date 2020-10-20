#include <Arduino.h>
#include <Globals.h>
#include <Data.h>


static struct Data::TOPOGRAPHY_16
{
    int array[Globals::numInputs][16];
    int threshold;
    int average_smooth;
} beat_topography_8, beat_topography_16, beat_regularity_16;

///////////////////////////// FUNCTIONS ///////////////////////////////

// ------------------- SMOOTHEN TOPOGRAPHY ARRAYS ---------------------
// ---------------- smoothen 16-bit array using struct ----------------
void Data::smoothen_dataArray(struct Data::TOPOGRAPHY_16(*struct_ptr), int instr)
{
    int entries = 0;
    int squared_sum = 0;
    int regular_sum = 0;

    // count entries and create squared sum:
    for (int j = 0; j < 16; j++)
    {
        if ((*struct_ptr).array[instr][j] > 0)
        {
            entries++;
            squared_sum += (*struct_ptr).array[instr][j] * (*struct_ptr).array[instr][j];
            regular_sum += (*struct_ptr).array[instr][j];
        }
    }

    regular_sum = regular_sum / entries;

    // calculate site-specific (squared) fractions of total:
    float squared_frac[16];
    for (int j = 0; j < 16; j++)
        squared_frac[j] =
            float((*struct_ptr).array[instr][j]) / float(squared_sum);

    // get highest frac:
    float highest_squared_frac = 0;
    for (int j = 0; j < 16; j++)
        highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

    // get "topography height":
    // divide highest with other entries and reset entries if ratio > 3:
    for (int j = 0; j < 16; j++)
        if (squared_frac[j] > 0)
            if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > (*struct_ptr).threshold)
            {
                (*struct_ptr).array[instr][j] = 0;
                entries -= 1;
            }

    (*struct_ptr).average_smooth = 0;
    // assess average topo sum for loudness
    for (int j = 0; j < 8; j++)
        (*struct_ptr).average_smooth += (*struct_ptr).array[instr][j];
    (*struct_ptr).average_smooth = int((float((*struct_ptr).average_smooth) / float(entries)) + 0.5);
}

// --------------- smoothen 8-bit array holding instrument ------------
void Data::smoothen_dataArray(int input_array[Globals::numInputs][8], int instr_in, int threshold_to_omit_entry)
{
    int entries = 0;
    int squared_sum = 0;
    int regular_sum = 0;

    // count entries and create squared sum:
    for (int j = 0; j < 8; j++)
    {
        if (input_array[instr_in][j] > 0)
        {
            entries++;
            squared_sum += input_array[instr_in][j] * input_array[instr_in][j];
            regular_sum += input_array[instr_in][j];
        }
    }

    regular_sum = regular_sum / entries;

    // calculate site-specific (squared) fractions of total:
    float squared_frac[8];
    for (int j = 0; j < 8; j++)
        squared_frac[j] =
            float(input_array[instr_in][j]) / float(squared_sum);

    // get highest frac:
    float highest_squared_frac = 0;
    for (int j = 0; j < 8; j++)
        highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

    // get "topography height":
    // divide highest with other entries and reset entries if ratio > 3:
    for (int j = 0; j < 8; j++)
        if (squared_frac[j] > 0)
            if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > threshold_to_omit_entry)
            {
                input_array[instr_in][j] = 0;
                entries -= 1;
            }
}

// ------------------------ smoothen 16-bit array ---------------------
void Data::smoothen_dataArray(int input_array[16], int threshold_to_omit_entry)
{
    int entries = 0;
    int squared_sum = 0;
    int regular_sum = 0;

    // count entries and create squared sum:
    for (int j = 0; j < 16; j++)
    {
        if (input_array[j] > 0)
        {
            entries++;
            squared_sum += input_array[j] * input_array[j];
            regular_sum += input_array[j];
        }
    }

    regular_sum = regular_sum / entries;

    // calculate site-specific (squared) fractions of total:
    float squared_frac[16];
    for (int j = 0; j < 16; j++)
        squared_frac[j] =
            float(input_array[j]) / float(squared_sum);

    // get highest frac:
    float highest_squared_frac = 0;
    for (int j = 0; j < 16; j++)
        highest_squared_frac = (squared_frac[j] > highest_squared_frac) ? squared_frac[j] : highest_squared_frac;

    // get "topography height":
    // divide highest with other entries and reset entries if ratio > 3:
    for (int j = 0; j < 16; j++)
        if (squared_frac[j] > 0)
            if (highest_squared_frac / squared_frac[j] > 3 || squared_frac[j] / highest_squared_frac > threshold_to_omit_entry)
            {
                input_array[j] = 0;
                entries -= 1;
            }
}