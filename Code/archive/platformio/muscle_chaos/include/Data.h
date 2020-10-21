#ifndef DATA_H
#define DATA_H
#include <Arduino.h>
#include <Globals.h>

class Data
{
public:
    struct TOPOGRAPHY_16;

    void smoothen_dataArray(struct Data::TOPOGRAPHY_16(*struct_ptr), int instr);

    void smoothen_dataArray(int input_array[Globals::numInputs][8], int instr_in, int threshold_to_omit_entry); // smoothen 8-bit array holding instrument

    void smoothen_dataArray(int input_array[16], int threshold_to_omit_entry); // smoothen 16-bit array
};

#endif