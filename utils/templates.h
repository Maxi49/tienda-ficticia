//
// Created by gimen on 03/11/2025.
//

#pragma once

#include <string>
#include <vector>
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

#include <type_traits>

// ---- showVectorInfo genérico y seguro ----
template<typename T>
void showVectorInfo(const std::vector<T>& v) {
    if (v.empty()) {
        std::cout << "(vacío)\n";
        return;
    }

    int index = 1;
    for (const auto& elem : v) {
        // Si es puntero, desreferenciamos de forma segura
        if constexpr (std::is_pointer_v<T>) {
            if (!elem) continue;

            // Si el puntero apunta a Product
            if constexpr (std::is_base_of_v<Product, std::remove_pointer_t<T>>) {
                std::cout << "[" << index++ << "] "
                          << "ID " << elem->getId() << " | "
                          << "Nombre: " << elem->getName() << " | "
                          << "Tipo: " << elem->type() << "\n";
            }
            // Si apunta a Client
            else if constexpr (std::is_base_of_v<Client, std::remove_pointer_t<T>>) {
                std::cout << "[" << index++ << "] "
                          << "ID " << elem->getId() << " | "
                          << "Nombre: " << elem->getName() << "\n";
            }
            // Si es puntero a otra cosa
            else {
                std::cout << "[" << index++ << "] " << *elem << "\n";
            }
        }
        // Si NO es puntero
        else {
            std::cout << "[" << index++ << "] " << elem << "\n";
        }
    }
}



