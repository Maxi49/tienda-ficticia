//
// Created by gimen on 03/11/2025.
//

#pragma once

#include <string>
#include "read_inputs.h"

template <typename T>
T readValue(const std::string& prompt);

template <>
inline std::string readValue<std::string>(const std::string& prompt) {
    return readStr(prompt);
}

template <>
inline int readValue<int>(const std::string& prompt) {
    return readInt(prompt);
}

template <>
inline float readValue<float>(const std::string& prompt) {
    return readFloat(prompt);
}

template<typename T>
void showVectorInfo(const std::vector<T>& v) {
    for (const auto& elem : v)
        std::cout << elem << " ";
    std::cout << std::endl;
}

