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

// ============================
// JSON auxiliar (opcional)
// ============================

json Client::to_json() const {
    json rentals = json::array();
    for (const auto& r : activeRentals)
        rentals.push_back({{"productId", r.productId}, {"qty", r.qty}});

    return {
        {"id", std::to_string(clientID)},
        {"name", name},
        {"activeRentals", rentals}
    };
}