#include "../../headers/products/game.h"
#include <iostream>
using nlohmann::json;

// Constructor
Game::Game(
    int id,
    const std::string& name,
    const std::string& genero,
    const std::string& description,
    float price,
    int totalStock,
    const std::string& platform,
    const std::string& players)
    : Product(id, name, genero, description, price, totalStock),
      platform(platform),
      players(players) {}

// Constructor de copia
Game::Game(const Game& other)
    : Product(other),
      platform(other.platform),
      players(other.players) {}

// Operador de asignación
Game& Game::operator=(const Game& other) {
    if (this != &other) {
        Product::operator=(other);
        platform = other.platform;
        players  = other.players;
    }
    return *this;
}

// Mostrar información
void Game::showInfo() const {
    std::string estado;
    if (availableStock == 0) {
        estado = "SIN STOCK";
    } else if (availableStock < totalStock) {
        estado = "DISPONIBLE (con alquileres)";
    } else {
        estado = "DISPONIBLE";
    }

    std::cout << "[GAME] "
              << name
              << " | Genero: " << genero
              << " | Plataforma: " << platform
              << " | Jugadores: " << players
              << " | Precio: $" << price
              << " | Stock total: " << totalStock
              << " | Disponible: " << availableStock
              << " | Estado: " << estado
              << std::endl;
}

// Serialización
json Game::to_json() const {
    json j = base_json_();
    j["type"]     = "game";
    j["platform"] = platform;
    j["players"]  = players;
    return j;
}

// Deserialización
Game Game::from_json(const json& j) {
    Game g(0, "", "", "", 0.0f, 0, "", "");
    Product::from_json_base(g, j);
    g.platform = j.value("platform", "");
    g.players  = j.value("players", "");
    return g;
}
