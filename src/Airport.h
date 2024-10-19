#include <string>

#pragma once
struct Airport {
    std::string code;
    std::string name;
    std::string city;
    std::string country;
    double latitude = 0;
    double longitude = 0;
    
    void print() {
        std::cout << code << ", " << name << ", " << city << ", "  << country << ", " << latitude << ", "  << longitude;
    }
};