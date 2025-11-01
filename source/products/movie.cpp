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

Movie Movie::from_json(const json& j) {
    // 1) Construimos un Movie “vacío” (valores neutros)
    Movie m(
        /*id*/           0,
        /*name*/         "",
        /*genero*/       "",
        /*description*/  "",
        /*price*/        0.0f,
        /*totalStock*/   0,
        /*director*/     "",
        /*durationMin*/  0
    );

    // 2) Cargamos el BLOQUE BASE con la inversa del base_json_()
    Product::from_json_base(m, j);

    // 3) Cargamos SOLO los campos propios de Movie (simétrico a tu to_json)
    m.director    = j.value("director", "");
    m.durationMin = j.value("durationMin", 0);

    return m;
}