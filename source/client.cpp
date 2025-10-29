#include "../headers/client.h"
#include <iostream>

// Constructor
Client::Client(int id, const std::string& name)
    : clientID(id), name(name) {}

// Destructor
Client::~Client() = default;

// Constructor de copia
Client::Client(const Client& other)
    : clientID(other.clientID),
      name(other.name),
      activeRentals(other.activeRentals) {}

// Operador de asignación
Client& Client::operator=(const Client& other) {
    if (this != &other) {
        clientID = other.clientID;
        name = other.name;
        activeRentals = other.activeRentals;
    }
    return *this;
}

// Mostrar información
void Client::showInfo() const {
    std::cout << "Cliente ID: " << clientID
              << " | Nombre: " << name
              << " | Alquileres activos: " << activeRentals.size()
              << std::endl;
}

// ============================
// Métodos de alquiler
// ============================

bool Client::addRental(int productId, int qty) {
    if (qty <= 0) return false;

    for (auto& r : activeRentals) {
        if (r.productId == productId) {
            r.qty += qty;
            return true;
        }
    }
    activeRentals.push_back({productId, qty});
    return true;
}

bool Client::removeRental(int productId, int qty) {
    for (auto it = activeRentals.begin(); it != activeRentals.end(); ++it) {
        if (it->productId == productId) {
            if (qty >= it->qty)
                activeRentals.erase(it);
            else
                it->qty -= qty;
            return true;
        }
    }
    return false;
}

json Client::to_json() const {
    json rentals = json::array();
    for (const auto& r : activeRentals) {
        rentals.push_back({ {"productId", r.productId}, {"qty", r.qty} });
    }
    return {
        {"id", std::to_string(clientID)},   // << IMPORTANTE: string
        {"name", name},
        {"rentals", rentals}
    };
}

Client Client::from_json(const json& j) {
    int id = 0;
    if (j.contains("id")) {
        if (j["id"].is_string()) id = std::stoi(j["id"].get<std::string>());
        else if (j["id"].is_number_integer()) id = j["id"].get<int>();
    }
    Client c(id, j.value("name", std::string{}));
    if (j.contains("rentals") && j["rentals"].is_array()) {
        for (const auto& r : j["rentals"]) {
            c.addRental(r.value("productId", 0), r.value("qty", 0));
        }
    }
    return c;
}