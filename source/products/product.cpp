#include "../../headers/products/products.h"
#include <iostream>   // std::cout, std::endl

// Constructor (igual al tuyo)
Product::Product(
    int id,
    const std::string& name,
    const std::string& genero,
    const std::string& description,
    float price,
    int totalStock)
    : id(id),
      name(name),
      genero(genero),
      description(description),
      price(price),
      totalStock(totalStock),
      availableStock(totalStock),
      rented(false),
      activeRentals{} {}

// Métodos de stock (tus implementaciones)
bool Product::canRent(int amountReq) const {
    return amountReq > 0 && availableStock >= amountReq;
}

bool Product::applyRent(const std::string& client_id, int amountReq) {
    if (!canRent(amountReq)) return false;
    availableStock -= amountReq;
    activeRentals[client_id] += amountReq;
    rented = (availableStock < totalStock);
    return true;
}

bool Product::applyReturn(const std::string& client_id, int amountReq) {
    if (amountReq <= 0) return false;
    auto it = activeRentals.find(client_id);
    if (it == activeRentals.end() || it->second < amountReq) return false;
    it->second -= amountReq;
    availableStock += amountReq;
    if (it->second == 0) activeRentals.erase(it);
    rented = (availableStock < totalStock);
    return true;
}

// ---- Serialización base ----
json Product::base_json_() const {
    json rentals = json::object();
    for (const auto& [cid, q] : activeRentals) rentals[cid] = q;

    return json{
        {"id",             std::to_string(id)}, // tu JSON_DB exige string
        {"name",           name},
        {"genero",         genero},
        {"description",    description},
        {"price",          price},
        {"totalStock",     totalStock},
        {"availableStock", availableStock},
        {"rented",         rented},
        {"activeRentals",  rentals}
    };
}

json Product::to_json() const {
    json j = base_json_();
    j["type"] = "product";
    return j;
}