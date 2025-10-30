#include "../../headers/products/movie.h"
#include <iostream>

// Constructor
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

// Destructor
Movie::~Movie() {}

// Constructor de copia
Movie::Movie(const Movie& other)
    : Product(other), // copia la parte base
      director(other.director),
      durationMin(other.durationMin) {}

// Operador de asignación por copia
Movie& Movie::operator=(const Movie& other) {
    if (this != &other) {
        // Asignar la parte base (usa el operator= implícito de Product)
        Product::operator=(other);

        // Asignar los campos propios
        director    = other.director;
        durationMin = other.durationMin;
    }
    return *this;
}

// Salida de información
void Movie::showInfo() const {
    std::string estado;
    if (availableStock == 0) {
        estado = "SIN STOCK";
    } else if (availableStock < totalStock) {
        estado = "DISPONIBLE (con alquileres)";
    } else {
        estado = "DISPONIBLE";
    }
    std::cout << "[MOVIE] "
              << name
              << " | Gonero: " << genero
              << " | Director: " << director
              << " | Duración: " << durationMin << " min"
              << " | Precio: $" << price
              << " | Stock total: " << totalStock
              << " | Disponible: " << availableStock
              << " | Estado: " << estado
              << std::endl;
}

json Movie::to_json() const {
    json j = base_json_();
    j["type"]        = "movie";
    j["director"]    = director;
    j["durationMin"] = durationMin;
    return j;
}