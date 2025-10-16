#pragma once

#include <string>

inline std::string format_id(char prefix, unsigned int counter) {
    std::string number = std::to_string(counter);
    if (number.size() < 3) {
        number.insert(0, 3 - number.size(), '0');
    }
    number.insert(number.begin(), prefix);
    return number;
}

inline std::string& create_id_for_product() {
    static unsigned int counter = 0;
    static std::string current_id;

    ++counter;
    current_id = format_id('P', counter);
    return current_id;
}

inline std::string& create_id_for_client() {
    static unsigned int counter = 0;
    static std::string current_id;

    ++counter;
    current_id = format_id('C', counter);
    return current_id;
}
