#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "single_include/nlohmann/json.hpp"
#include "../db_utils/db_enum_aliases.h"
#include "../db_utils/json_db_utils.h"

using json = nlohmann::json;

struct RentalLine {
    int productId;
    int qty;
};

class Client {
private: 
    JSON_DB db_{Alias::Clients};
    int clientID;
    std::string name;
    std::vector<RentalLine> activeRentals; // productos alquilados actualmente

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
    const std::string& getName() const { return name; }
    const std::vector<RentalLine>& getActiveRentals() const { return activeRentals; }

    // Mostrar información
    void showInfo() const;

    // Métodos para gestionar alquileres
    bool addRental(int productId, int qty);
    bool removeRental(int productId, int qty);

    // ---- Serialización ----
    nlohmann::json to_json() const;
    static Client from_json(const nlohmann::json& j);
};
