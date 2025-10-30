#pragma once    
#include <string>
#include "products.h"


class Game : public Product {
    std::string platform;
    std::string players;
public:
    Game(int id, const std::string& name, const std::string& genero,
         const std::string& description, float price, int totalStock,
         const std::string& platform, const std::string& players);

    ~Game();
    Game(const Game& other);
    Game& operator=(const Game& other);

    std::string type() const override { return "game"; }
    nlohmann::json to_json() const override;
    void showInfo() const override;
};