#include "cobfile/c1cobfile.h"
#include "endianlove.h"

static std::string read_string(std::istream &in) {
    uint16_t length = read8(in);
    if (length == 255) {
        length = read16le(in);
    }
    std::vector<uint8_t> script(length + 1);
    script.back() = '\0';
    in.read((char*)script.data(), length);
    return { (char*)script.data() };
}

c1cobfile read_c1cobfile(std::istream &in) {
    c1cobfile cob;
    
    uint16_t version = read16le(in);
    assert(version == 1);
    
    cob.quantity_available = read16le(in);
    cob.expiration_month = read32le(in);
    cob.expiration_day = read32le(in);
    cob.expiration_year = read32le(in);
    
    uint16_t num_object_scripts = read16le(in);
    uint16_t num_install_scripts = read16le(in);

    cob.quantity_used = read32le(in);
    
    for (size_t i = 0; i < num_object_scripts; ++i) {
        cob.object_scripts.push_back(read_string(in));
    }
    for (size_t i = 0; i < num_install_scripts; ++i) {
        cob.install_scripts.push_back(read_string(in));
    }
    
    cob.picture_width = read32le(in);
    cob.picture_height = read32le(in);
    uint16_t unknown_always_picture_width = read16le(in);
     // ABK- Egg Gender.cob has it zeroed
    assert(unknown_always_picture_width == 0 || unknown_always_picture_width == cob.picture_width);
    
    cob.picture_data.resize(cob.picture_width * cob.picture_height);
    in.read((char*)cob.picture_data.data(), cob.picture_data.size());
    
    cob.name = read_string(in);
    
    return cob;
}