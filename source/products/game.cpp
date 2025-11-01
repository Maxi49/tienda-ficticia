#include "../../headers/products/game.h"
#include <iostream>

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

// Destructor
Game::~Game() {}

// Constructor de copia
Game::Game(const Game& other)
    : Product(other), // copia la parte base
      platform(other.platform),
      players(other.players) {}

// Operador de asignación por copia
Game& Game::operator=(const Game& other) {
    if (this != &other) {
        // Copiar la parte base
        Product::operator=(other);

        // Copiar los miembros propios
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

json Game::to_json() const {
    json j = base_json_();
    j["type"]     = "game";
    j["platform"] = platform;
    j["players"]  = players;
    return j;
}

Game Game::from_json(const json& j) {
    Game g(
        /*id*/           0,
        /*name*/         "",
        /*genero*/       "",
        /*description*/  "",
        /*price*/        0.0f,
        /*totalStock*/   0,
        /*platform*/     "",
        /*players*/      ""
    );

    Product::from_json_base(g, j);       // ← bloque base
    // Propios de Game (simétrico a tu to_json)
    g.platform = j.value("platform", "");
    g.players  = j.value("players", "");

    return g;
}