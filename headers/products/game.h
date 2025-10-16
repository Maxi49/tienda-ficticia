#pragma once    
#include <string>
#include "products.h"

class Game : public Product {
private:
    std::string platform;
    std::string players;

public:
    // Constructor
    Game(int id,
         const std::string& name,
         const std::string& genero,
         const std::string& description,
         float price,
         int totalStock,
         const std::string& platform,
         const std::string& players);

    // Destructor de la clase
    ~Game();

    // Constructor de copia
    Game(const Game& other);

    // Operador de asignación por copia
    Game& operator=(const Game& other);

    // --- Getters ---
    std::string getPlatform() const { return platform; }
    std::string getPlayers() const { return players; }

    // --- Setters ---
    void setPlatform(const std::string& p) { platform = p; }
    void setPlayers(const std::string& p) { players = p; }

    // --- Sobrescrituras ---
    std::string type() const override;
    void showInfo() const override;
};

