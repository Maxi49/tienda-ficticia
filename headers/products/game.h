#pragma once    
#include <string>
#include "products.h"
#include "single_include/nlohmann/json.hpp"


/*
    Clase Game (hereda de Product)
    Representa un videojuego dentro de la tienda.
    Hereda todos los atributos comunes de Product y agrega los suyos propios
    Además redefine algunos métodos para adaptarse a los videojuegos:
      type(): devuelve "game" para identificar el tipo.
      to_json(): convierte el objeto a formato JSON para guardarlo en archivo.
      from_json(): crea un Game a partir de datos guardados en JSON.
      showInfo(): muestra por consola la información del juego.
*/
class Game : public Product {
    std::string platform;
    std::string players; 

public:
    /*
        Constructor: inicializa todos los campos, incluyendo los heredados de Product.
        Se usa cuando se crea un nuevo videojuego.
    */
    Game(int id, const std::string& name, const std::string& genero,
         const std::string& description, float price, int totalStock,
         const std::string& platform, const std::string& players);

    Game(const Game& other); // Constructor de copia
    Game& operator=(const Game& other); // Operador de asignación

    // Devuelve el tipo de producto (sirve para distinguir entre "game" y "movie")
    std::string type() const override { return "game"; }

    // Convierte el objeto a JSON para guardarlo en archivo
    nlohmann::json to_json() const override;

    // Crea un objeto Game desde un JSON (al cargar desde disco)
    static Game from_json(const nlohmann::json& j);

    // Muestra toda la información del videojuego (usa operator<< de Product y sus propios campos)
    void showInfo() const override;
};
