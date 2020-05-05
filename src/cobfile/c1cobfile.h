#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

struct c1cobfile {
    std::string name;
    
    uint16_t quantity_available;
    uint16_t quantity_used;
    
    uint32_t expiration_month;
    uint32_t expiration_day;
    uint32_t expiration_year;
    
    std::vector<std::string> object_scripts;
    std::vector<std::string> install_scripts;
    
    uint32_t picture_width;
    uint32_t picture_height;
    
    std::vector<uint8_t> picture_data;
};

c1cobfile read_c1cobfile(std::istream &in);