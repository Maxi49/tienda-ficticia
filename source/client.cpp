#include "../headers/client.h"
#include <iostream>

//Constructor
Client::Client(int id, const std::string& name)
    : clientID(id), name(name) {
}

void Client::showInfo() const {
    std::cout << "Cliente ID: " << clientID
              << " | Nombre: " << name
              << " | Alquileres activos: " << activeRentals.size()
              << std::endl;
}

//Metodos de alquilers
bool Client::addRental(int productId, int qty) {
    // Validaciones mínimas para evitar estados inválidos:
    if (productId <= 0 || qty <= 0) {
        std::cerr << "[Client::addRental] productId/qty inválidos (pid="
                  << productId << ", qty=" << qty << ")\n";
        return false;
    }

    // Si ya existe una línea para ese producto, acumula cantidades.
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
    if (productId <= 0 || qty <= 0) {
        std::cerr << "[Client::removeRental] productId/qty inválidos (pid="
                  << productId << ", qty=" << qty << ")\n";
        return false;
    }

    for (auto it = activeRentals.begin(); it != activeRentals.end(); ++it) {
        if (it->productId == productId) {
            if (qty > it->qty) {
                std::cerr << "[Client::removeRental] intenta devolver " << qty
                          << " pero solo tiene " << it->qty
                          << " del producto " << productId << "\n";
                return false;
            }
            if (qty == it->qty) {
                activeRentals.erase(it);  // borra la línea completa
            } else {
                it->qty -= qty;           // descuenta cantidad
            }
            return true;
        }
    }

    std::cerr << "[Client::removeRental] el cliente no tiene el producto "
              << productId << " alquilado.\n";
    return false;
}

// Serialización

json Client::to_json() const {
    // Convierte las líneas de alquiler a un array JSON simple
    json rentals = json::array();
    for (const auto& r : activeRentals) {
        rentals.push_back({ {"productId", r.productId}, {"qty", r.qty} });
    }

    // Importante: El id va como string porque así lo espera JSON_DB
    return {
        {"id", std::to_string(clientID)},
        {"name", name},
        {"rentals", rentals}
    };
}

Client Client::from_json(const json& j) {
    // Lee el id tolerando string o número
    int id = 0;
    try {
        if (j.contains("id")) {
            if (j["id"].is_string()) {
                id = std::stoi(j["id"].get<std::string>());
            } else if (j["id"].is_number_integer()) {
                id = j["id"].get<int>();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Client::from_json] id inválido: " << e.what()
                  << " (se usará id=0)\n";
        id = 0;
    }

    Client c(id, j.value("name", std::string{}));

    // Reconstruye alquileres si hay array 'rentals'
    if (j.contains("rentals") && j["rentals"].is_array()) {
        for (const auto& r : j["rentals"]) {
            const int pid = r.value("productId", 0);
            const int q   = r.value("qty", 0);
            // Reutiliza la validación de addRental
            if (!c.addRental(pid, q)) {
                std::cerr << "[Client::from_json] línea inválida (pid="
                          << pid << ", qty=" << q << ")\n";
            }
        }
    }
    return c;
}

// Persistencia

bool Client::upsert(const Client& c) {
    try {
        JSON_DB db{Alias::Clients};
        json obj = c.to_json();

        const std::string id = obj["id"].get<std::string>();
        // Inserta si no existe, actualiza si ya está
        return db.find_by_id(id) ? db.update_by_id(std::move(obj))
                                 : db.insert(std::move(obj));
    } catch (const std::exception& e) {
        std::cerr << "[Client::upsert] Error: " << e.what() << "\n";
        return false;
    }
}
