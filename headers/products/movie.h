#pragma once

#include <string>
#include "products.h"
#include "single_include/nlohmann/json.hpp"

class Movie : public Product {
private:
    std::string director;
    int durationMin; // duración en minutos

public:
    // Constructor
    Movie(int id,
          const std::string& name,
          const std::string& genero,
          const std::string& description,
          float price,
          int totalStock,
          const std::string& director,
          int durationMin);

    // Destructor de la clase
    ~Movie();

    // Constructor de copia
    Movie(const Movie& other);

    // Operador de asignación por copia
    Movie& operator=(const Movie& other);

    // --- Getters ---
    std::string getDirector() const { return director; }
    int getDurationMin() const { return durationMin; }

    // --- Setters ---
    void setDirector(const std::string& newDirector) { director = newDirector; }
    void setDurationMin(int newDuration) { durationMin = newDuration; }

    // --- Sobrescrituras ---
    std::string type() const override { return "movie"; }
    nlohmann::json to_json() const override;
    static Movie from_json(const nlohmann::json& j);
    void showInfo() const override;
};
