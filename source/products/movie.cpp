#include "../../headers/products/movie.h"
#include <iostream>
using nlohmann::json;

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

Movie::Movie(const Movie& other)
    : Product(other),
      director(other.director),
      durationMin(other.durationMin) {}

Movie& Movie::operator=(const Movie& other) {
    if (this != &other) {
        Product::operator=(other);
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

// Serialización
json Movie::to_json() const {
    json j = base_json_();
    j["type"]        = "movie";
    j["director"]    = director;
    j["durationMin"] = durationMin;
    return j;
}

// Deserialización
Movie Movie::from_json(const json& j) {
    Movie m(0, "", "", "", 0.0f, 0, "", 0);
    Product::from_json_base(m, j);
    m.director    = j.value("director", "");
    m.durationMin = j.value("durationMin", 0);
    return m;
}
