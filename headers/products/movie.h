#pragma once

#include <string>
#include "products.h"
#include "single_include/nlohmann/json.hpp"

/*
    Clase Movie (hereda de Product)
    Representa una película dentro de la tienda.

    Hereda todos los atributos comunes de Product (id, nombre, genero, precio, stock)
    y agrega los suyos propios:
      - director: nombre del director de la película
      - durationMin: duración total en minutos

    Además redefine algunos métodos para adaptarse a las películas:
      - type(): devuelve "movie" para identificar el tipo.
      - to_json(): convierte el objeto a formato JSON para guardarlo en archivo.
      - from_json(): crea una película a partir de un JSON (al leer desde disco).
      - showInfo(): muestra por consola toda la información de la película.
*/
class Movie : public Product {
private:
    std::string director;  // Nombre del director
    int durationMin;       // Duración de la película en minutos

public:
    /*
        Constructor: inicializa todos los campos, incluyendo los heredados de Product.
        Se usa al crear una nueva película.
    */
    Movie(int id,
          const std::string& name,
          const std::string& genero,
          const std::string& description,
          float price,
          int totalStock,
          const std::string& director,
          int durationMin);
          
    // Constructor de copia
    Movie(const Movie& other);

    // Operador de asignación por copia
    Movie& operator=(const Movie& other);

    // Sobrescrituras de Product
    std::string type() const override { return "movie"; } // Identifica el tipo de producto
    nlohmann::json to_json() const override;              // Convierte la película a formato JSON
    static Movie from_json(const nlohmann::json& j);      // Crea una película a partir de un JSON

    // Muestra por consola la información completa (usa operator<< de Product +  campos de pelicula)
    void showInfo() const override;
};
