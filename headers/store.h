#pragma once
#include <unordered_map>
#include <memory>
#include <functional>
#include "../db_utils/json_db_utils.h"
#include "../db_utils/db_enum_aliases.h"
#include "products/game.h"
#include "products/movie.h"
#include "client.h"

class Store {
    // Archivos JSON usados por el sistema
    JSON_DB games_{Alias::Games};
    JSON_DB movies_{Alias::Movies};
    JSON_DB clients_db_{Alias::Clients};

    // Datos cargados en memoria (uso de smart pointers → sin delete manual)
    std::unordered_map<int, std::unique_ptr<Product>> catalog_;
    std::unordered_map<int, std::unique_ptr<Client>>  clients_;

    // ---------- Helpers internos ----------
    static bool upsert_json_(nlohmann::json obj, JSON_DB& db);
    bool upsert_product_(const Product& p);

    // Función genérica para agregar un producto (usa lambdas para crear Game/Movie)
    bool add_product_(int id, std::function<std::unique_ptr<Product>()> make);

    // Carga genérica desde un JSON_DB con una factory (lambda)
    void load_from_db_(
        JSON_DB& db,
        const std::function<std::unique_ptr<Product>(const nlohmann::json&)>& factory
    );

public:
    // ---------- Carga / Guardado ----------
    void load_from_disk();   // Lee todos los JSON y llena memoria
    void save_to_disk();     // Guarda los productos del catálogo

    // ---------- Altas ----------
    bool add_game(int id, const std::string& name, const std::string& genero,
                  const std::string& desc, float price, int stock,
                  const std::string& platform, const std::string& players);

    bool add_movie(int id, const std::string& name, const std::string& genero,
                   const std::string& desc, float price, int stock,
                   const std::string& director, int durationMin);

    bool add_client(int id, const std::string& name);

    // ---------- Consultas ----------
    Product* find_product(int id);
    Client*  find_client(int id);

    void list_products() const;
    void list_clients()  const;

    // Usado por TransactionService para guardar snapshots
    bool upsert(const Product& p) { return upsert_product_(p); }
};
