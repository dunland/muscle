#pragma once
#include <vector>
#include <Score/Score.h>

class Instrument;

class JSON
{
    public:
    
    static void compose_and_send_json(std::vector<Instrument *> instruments);

    static void save_settings_to_SD(std::vector<Instrument *> instruments);

    static void read_sensitivity_data_from_SD(std::vector<Instrument *> instruments);

};