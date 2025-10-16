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

// Método type()
std::string Game::type() const {
    return "game";
}

// Mostrar información
void Game::showInfo() const {
    std::cout << "[GAME] "
              << name
              << " | Género: " << genero
              << " | Plataforma: " << platform
              << " | Jugadores: " << players
              << " | Precio: $" << price
              << " | Stock total: " << totalStock
              << " | Disponible: " << availableStock
              << (rented ? " | Estado: ALQUILADO" : " | Estado: DISPONIBLE")
              << std::endl;
}
