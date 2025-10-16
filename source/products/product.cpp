#include "../../headers/products/products.h"
#include <iostream>   // std::cout, std::endl

// Constructor
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
      active_rentals_{} {}

// Métodos internos de stock

bool Product::canRent(int amountReq) const {
    // Se puede alquilar si la cantidad solicitada es válida
    // y hay suficiente stock disponible
    return amountReq > 0 && availableStock >= amountReq && rented == false;
}

bool Product::applyRent(const std::string& client_id, int amountReq) {
    if (!canRent(amountReq)) return false;

    // Actualiza stock y registro de alquileres activos
    availableStock -= amountReq;
    active_rentals_[client_id] += amountReq;

    // Si hay menos disponibles que el total, significa que hay algo alquilado
    rented = (availableStock < totalStock);
    return true;
}

bool Product::applyReturn(const std::string& client_id, int amountReq) {
    if (amountReq <= 0) return false;

    auto it = active_rentals_.find(client_id);
    if (it == active_rentals_.end() || it->second < amountReq) return false;

    // Revertir cantidades
    it->second -= amountReq;
    availableStock += amountReq;

    if (it->second == 0) {
        active_rentals_.erase(it);
    }

    // Si el stock disponible vuelve a ser igual al total, ya no hay nada alquilado
    rented = (availableStock < totalStock);
    return true;
}

// Salida de información
void Product::showInfo() const {
    
    std::cout << "ID: " << id
              << " | Nombre: " << name
              << " | Género: " << genero
              << " | Precio: $" << price
              << " | Stock total: " << totalStock
              << " | Disponible: " << availableStock
              << (rented ? " | Estado: ALQUILADO" : " | Estado: DISPONIBLE")
              << std::endl;
}