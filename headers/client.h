#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "single_include/nlohmann/json.hpp"
#include "../db_utils/db_enum_aliases.h"
#include "../db_utils/json_db_utils.h"

using json = nlohmann::json;

// Representa una fila de alquiler: producto y cantidad
// Sirve para no tener datos sueltos en el json
struct RentalLine {
    int productId{};
    int qty{};
    //Cnstruir directo o comparar en búsquedas
    RentalLine() = default;
    RentalLine(int pid, int q) : productId(pid), qty(q) {}
    bool operator==(const RentalLine& other) const {
        return productId == other.productId && qty == other.qty;
    }
};

class Client {
private:
    int clientID;
    std::string name;
    std::vector<RentalLine> activeRentals; // alquileres actuales (producto -> cantidad)

public:
    // client.h
    ~Client()=default;     
    // Constructores / asignación 
    Client(int id, const std::string& name);
    Client(const Client& other) = default;
    Client& operator=(const Client& other) = default;

    // Getters 
    int getId() const noexcept { return clientID; }
    const std::string& getName() const noexcept { return name; }
    const std::vector<RentalLine>& getActiveRentals() const noexcept { return activeRentals; }

    // Muestra información general del cliente (id, nombre y cantidad de alquileres activos)
    void showInfo() const;

    // Agrega o actualiza un alquiler
    bool addRental(int productId, int qty);

    // Quita un alquiler
    bool removeRental(int productId, int qty);

    // Convierte el cliente a formato JSON para guardar en archivo.
    json to_json() const;

    // Crea un cliente a partir de datos leídos desde JSON.
    static Client from_json(const json& j);

    // Guarda o actualiza el cliente en db/clients.json (según su id).
    static bool upsert(const Client& c);
};
