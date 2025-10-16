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
protected :
    int id;
    std::string name;
    std::string genero;
    std::string description;
    float price;
    int totalStock; 
    int availableStock;
    bool rented;
    std::unordered_map<std::string, int> activeRentals;

public:
    // Constructor
    Product(int id, const std::string& name, const std::string& genero,
            const std::string& description, float price, int totalStock);

    // Destructor virtual (necesario si heredas)
    virtual ~Product() = default;

    // Métodos virtuales para que las clases hijas los redefinan
    virtual std::string type() const = 0;
    virtual void showInfo() const;

    // Métodos internos de stock
    bool canRent(int qty) const;
    bool applyRent(const std::string& client_id, int qty);
    bool applyReturn(const std::string& client_id, int qty);

    // Getters
    int getId() const { return id; }
    std::string getName() const { return name; }
    int getAvailableStock() const { return availableStock; }
    float getPrice() const { return price; }
};

