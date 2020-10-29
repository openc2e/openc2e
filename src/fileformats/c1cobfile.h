#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

#include "Image.h"

struct c1cobfile {
    std::string name;
    
    uint16_t quantity_available;
    uint16_t quantity_used;
    
    uint32_t expiration_month;
    uint32_t expiration_day;
    uint32_t expiration_year;
    
    std::vector<std::string> object_scripts;
    std::vector<std::string> install_scripts;
    
    Image picture;
};

c1cobfile read_c1cobfile(const std::string &path);
c1cobfile read_c1cobfile(std::istream &in);