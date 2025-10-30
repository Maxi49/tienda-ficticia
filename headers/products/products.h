#pragma once
#include <optional>
#include <string>
#include <utility>
#include <unordered_map>

#include "../../headers/single_include/nlohmann/json.hpp"
using json = nlohmann::json;
#include "../../db_utils/db_enum_aliases.h"
#include "../../db_utils/json_db_utils.h"

class Product {
protected:
    int id;
    std::string name;
    std::string genero;
    std::string description;
    float price;
    int totalStock;
    int availableStock;
    bool rented;
    std::unordered_map<std::string, int> activeRentals;

    // JSON común
    nlohmann::json base_json_() const;

public:
    Product(int id, const std::string& name, const std::string& genero,
            const std::string& description, float price, int totalStock);

    // Getters mínimos
    int getId() const { return id; }
    const std::string& getName() const { return name; }

    // Stock / rentals
    bool canRent(int amountReq) const;
    bool applyRent(const std::string& client_id, int amountReq);
    bool applyReturn(const std::string& client_id, int amountReq);

    // Polimorfismo
    virtual std::string type() const = 0;           // "game" / "movie"
    virtual nlohmann::json to_json() const;         // puede sobreescribirse
    virtual void showInfo() const = 0;              // ya lo usás
    virtual ~Product() = default;
};