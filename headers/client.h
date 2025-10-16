#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "single_include/nlohmann/json.hpp"
#include "../db_utils/db_enum_aliases.h"
#include "../db_utils/json_db_utils.h"

using json = nlohmann::json;

struct Rental {
    int productId;
    int qty;
};

class Client {
private: 
    JSON_DB db_{Alias::Clients};
    int clientID;
    std::string name;
    std::vector<Rental> activeRentals; // productos alquilados actualmente

public:
    // Constructor
    Client(int id, const std::string& name);

    // Destructor
    ~Client();

    // Constructor de copia
    Client(const Client& other);

    // Operador de asignación
    Client& operator=(const Client& other);

    // Getters
    int getId() const { return clientID; }
    std::string getName() const { return name; }

    // Mostrar información
    void showInfo() const;

    // Métodos para gestionar alquileres
    bool addRental(int productId, int qty);
    bool removeRental(int productId, int qty);

    // (Opcional) convertir a JSON si después querés persistir
    json to_json() const;
};
