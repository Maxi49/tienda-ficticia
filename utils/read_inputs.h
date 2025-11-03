//
// Created by gimen on 03/11/2025.
//

#pragma once
#include <iostream>
#include <limits>
#include <string>

inline int readInt(const std::string& prompt) {
    int x;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada inválida.\n";
    }
}

inline std::string readStr(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin >> std::ws, s);
    return s;
}

inline float readFloat(const std::string& prompt) {
    float x;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada inválida.\n";
    }
}
