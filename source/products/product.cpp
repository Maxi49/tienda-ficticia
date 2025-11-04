#include "../../headers/products/products.h"
#include <iostream>  

// Constructor
Product::Product(
    int id,
    const std::string& name,
    const std::string& genero,
    const std::string& description,
    float price,
    int totalStock
    )
    : id(id),
      name(name),
      genero(genero),
      description(description),
      price(price),
      totalStock(totalStock),
      availableStock(totalStock),
      rented(false),
      activeRentals{} {}

void Product::showInfo() const {
    std::string estado;
    if (availableStock == 0) {
        estado = "SIN STOCK";
    } else if (availableStock < totalStock) {
        estado = "DISPONIBLE (con alquileres)";
    } else {
        estado = "DISPONIBLE";
    }
    std::cout << " | Tipo " << type()
              << " | ID " << id
              << " | nombre " << name
              << " | Genero: " << genero
              << " | Precio: $" << price
              << " | Stock total: " << totalStock
              << " | Disponible: " << availableStock
              << " | Estado: " << estado
              << std::endl;
}


// canRent(amountReq)
// Devuelve true si la cantidad pedida es > 0 y hay stock suficiente.

bool Product::canRent(const int amountReq) const {
    return amountReq > 0 && availableStock >= amountReq;
}

/*
applyRent(client_id, amountReq)
Descuenta stock y registra el alquiler del cliente en el mapa
'activeRentals' (clave: id de cliente, valor: cantidad).
Marca 'rented' si el stock disponible bajó respecto al total.
*/
bool Product::applyRent(const std::string& client_id, int amountReq) {
    if (!canRent(amountReq)) return false;
    availableStock -= amountReq;
    activeRentals[client_id] += amountReq;
    rented = (availableStock < totalStock);
    return true;
}

/*
applyReturn(client_id, amountReq)
Revierte un alquiler es decir suma stock y reduce/borra la línea del cliente.
Si la cantidad a devolver no es válida, retorna false.
*/
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

/*
base_json_()
Define el "bloque base" del producto a JSON.
Punto importante: el id se guarda como STRING para que JSON_DB lo acepte.
Además, convierte 'activeRentals' (map<string,int>) a un objeto JSON
con pares "client_id" : cantidad.
*/

json Product::base_json_() const {
    // Serializamos el mapa de alquileres como un objeto JSON:
    // { "123": 2, "45": 1, ... }
    json rentals = json::object();
    for (const auto& [cid, q] : activeRentals) rentals[cid] = q; // <- agrega cada par (cliente -> qty)

    return json{
        {"id",             std::to_string(id)}, // JSON_DB exige string
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

// to_json()
// Devuelve el JSON base + el "type" genérico. Las derivadas
// (Game y Movie) lo sobrescriben agregando sus campos propios.

json Product::to_json() const {
    json j = base_json_();
    j["type"] = "product";
    return j;
}

// parse_id_()
// Helper  para leer 'id' tolerando string o número.
// Si el string no es convertible a int, avisa y retorna 0.
static int parse_id_(const nlohmann::json& j) {
    try {
        const auto& idv = j.at("id");
        if (idv.is_string()) {
            return std::stoi(idv.get<std::string>());
        }
        return idv.get<int>(); // si vino como número entero
    } catch (const std::exception& e) {
        std::cerr << "[Product::parse_id_] id inválido: " << e.what() << " (se usa 0)\n";
        return 0;
    }
}

/*
    from_json_base(p, j)
    Inversa de base_json_():
    Carga en 'p' los campos comunes desde el JSON. Responde a faltantes,
    usa valores por defecto si algo no está. Reconstruye el mapa
    'activeRentals' recorriendo el objeto JSON de alquileres.
*/
void Product::from_json_base(Product& p, const nlohmann::json& j) {
    // Campos “fijos” del producto
    p.id          = parse_id_(j);
    p.name        = j.value("name", "");
    p.genero      = j.value("genero", "");
    p.description = j.value("description", "");
    p.price       = j.value("price", 0.0f);
    p.totalStock  = j.value("totalStock", 0);

    // Estado (si falta availableStock, se asume igual al total)
    p.availableStock = j.value("availableStock", p.totalStock);
    p.rented         = j.value("rented", false);

    /*  
    Reconstrucción de 'activeRentals'
    Espera un objeto del estilo:
      "activeRentals": { "123": 2, "45": 1 }
    Recorremos pares clave->valor con un iterador de objeto JSON.
    Si alguna cantidad no es int, se omite con un warning.
    */
    p.activeRentals.clear();
    if (j.contains("activeRentals") && j["activeRentals"].is_object()) {
        for (auto it = j["activeRentals"].begin(); it != j["activeRentals"].end(); ++it) {
            const std::string client_id = it.key();
            try {
                // Si el valor no es un entero válido, lanzará excepción y lo saltamos.
                int qty = it.value().get<int>();
                if (qty > 0) {
                    p.activeRentals[client_id] = qty;
                }
            } catch (const std::exception& e) {
                std::cerr << "[Product::from_json_base] alquiler inválido (client_id="
                          << client_id << "): " << e.what() << "\n";
                // seguimos con los demás sin cortar
            }
        }
    }
}