#pragma once
#include "products.h"
#include <string>

class Game : public Product {
private:
    std::string platform;
    std::string players;

public:
    Game(int id, const std::string& name, const std::string& genero,
         const std::string& description, float price, int totalStock,
         const std::string& platform, const std::string& players);

    // Getters
    std::string getPlatform() const;
    int getPlayers() const;

    // Setters
    void setPlatform(const std::string& p);
    void setPlayers(std::string p);

        // Sobrescribe métodos virtuales
    std::string type() const override;
    void showInfo() const override;
};
