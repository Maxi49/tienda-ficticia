
#include "../../headers/products/game.h"
#include <iostream>

Game::Game(int id,
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
};

