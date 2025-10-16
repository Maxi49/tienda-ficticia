#include "../../headers/products/movie.h"
#include <iostream>

Movie::Movie(int id,
    const std::string& name,
    const std::string& genero,
    const std::string& description,
    float price,
    int totalStock,
    const std::string& director,
    int durationMin)
    : Product(id, name, genero, description, price, totalStock),
      director(director),
      durationMin(durationMin) {}

void Movie::showInfo() const {
    std::cout << "[MOVIE] "
              << name
              << " | Género: " << genero
              << " | Director: " << director
              << " | Duración: " << durationMin << " min"
              << " | Precio: $" << price
              << " | Stock total: " << totalStock
              << " | Disponible: " << availableStock
              << (rented ? " | Estado: ALQUILADA" : " | Estado: DISPONIBLE")
              << std::endl;
};
