#pragma once
#include <vector>

class Instrument;

class JSON
{
    public:
    
    static void compose_and_send_json(std::vector<Instrument *> instruments);
};