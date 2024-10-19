#include <string>

#pragma once
struct Airport {
    std::string code;
    std::string name;
    std::string city;
    std::string country;
    double latitude;
    double longitude;

    void print() {
        std::cout << code;
        std::cout << ", ";

        std::cout << name;
        std::cout << ", ";

        std::cout << city;
        std::cout << ", ";

        std::cout << country;
        std::cout << ", ";

        std::cout << latitude;
        std::cout << ", ";

        std::cout << longitude;
        std::cout << ", ";
        
    }
};