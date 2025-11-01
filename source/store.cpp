#include "../headers/store.h"
#include "single_include/nlohmann/json.hpp"
#include <iostream>
using nlohmann::json;

// Inserta o actualiza un objeto JSON en la base correspondiente
bool Store::upsert_json_(json obj, JSON_DB& db) {
    const std::string id = obj["id"].get<std::string>();
    return db.find_by_id(id) ? db.update_by_id(std::move(obj))
                             : db.insert(std::move(obj));
}

// Elige a qué archivo JSON guardar según el tipo de producto
bool Store::upsert_product_(const Product& p) {
    const auto j = p.to_json();
    const std::string t = p.type();
    if (t == "game")  return upsert_json_(j, games_);
    if (t == "movie") return upsert_json_(j, movies_);
    return true;
}

// Agrega un nuevo producto al catálogo (usa make_unique para crear smart pointer)
bool Store::add_product_(int id, std::function<std::unique_ptr<Product>()> make) {
    if (catalog_.count(id)) return false;     // id duplicado
    auto ptr = make();                        // crea el producto dinámicamente
    if (!ptr) return false;
    if (!upsert_product_(*ptr)) return false; // guarda en JSON
    catalog_[id] = std::move(ptr);            // guarda en memoria (move por unique_ptr)
    return true;
}

// Carga genérica desde un JSON_DB (usa factory lambda para crear cada tipo)
void Store::load_from_db_(
    JSON_DB& db,
    const std::function<std::unique_ptr<Product>(const json&)>& factory
) {
    const json& arr = db.all();
    if (!arr.is_array()) return;
    for (const auto& j : arr) {
        if (!j.is_object()) continue;
        if (auto p = factory(j)) {
            catalog_[p->getId()] = std::move(p);
        }
    }
}

// ---------- Carga desde disco ----------
void Store::load_from_disk() {
    // Cargar todos los juegos
    load_from_db_(games_, [](const json& j) -> std::unique_ptr<Product> {
        Game g = Game::from_json(j);
        return std::make_unique<Game>(g); // crea smart pointer a copia de g
    });

    // Cargar todas las películas
    load_from_db_(movies_, [](const json& j) -> std::unique_ptr<Product> {
        Movie m = Movie::from_json(j);
        return std::make_unique<Movie>(m);
    });

    // Cargar todos los clientes
    const json& arr = clients_db_.all();
    if (arr.is_array()) {
        for (const auto& j : arr) {
            if (!j.is_object()) continue;
            Client c = Client::from_json(j);
            clients_[c.getId()] = std::make_unique<Client>(c);
        }
    }
}

// ---------- Guardado ----------
void Store::save_to_disk() {
    // Guarda el estado actual del catálogo (sin borrar ausentes)
    for (const auto& [id, ptr] : catalog_)
        if (ptr) upsert_product_(*ptr);
    // Los clientes se guardan cuando se crean o en transacciones
}
// Helper para búsquedas case-insensitive
static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return s;
}

// ---------- Altas ----------
bool Store::add_game(int id, const std::string& name, const std::string& genero,
                     const std::string& desc, float price, int stock,
                     const std::string& platform, const std::string& players) {
    // Lambda que crea un Game; se pasa al helper genérico
    return add_product_(id, [&] {
        return std::make_unique<Game>(id, name, genero, desc, price, stock, platform, players);
    });
}

bool Store::add_movie(int id, const std::string& name, const std::string& genero,
                      const std::string& desc, float price, int stock,
                      const std::string& director, int durationMin) {
    // Lambda que crea una Movie
    return add_product_(id, [&] {
        return std::make_unique<Movie>(id, name, genero, desc, price, stock, director, durationMin);
    });
}

bool Store::add_client(int id, const std::string& name) {
    if (clients_.count(id)) return false;
    auto c = std::make_unique<Client>(id, name); // make_unique = new + delete automático
    if (!Client::upsert(*c)) return false;       // persistencia inmediata
    clients_[id] = std::move(c);
    return true;
}

// ---------- Consultas ----------
Product* Store::find_product(int id) {
    auto it = catalog_.find(id);
    return (it == catalog_.end()) ? nullptr : it->second.get(); // get() devuelve el puntero crudo
}

Client* Store::find_client(int id) {
    auto it = clients_.find(id);
    return (it == clients_.end()) ? nullptr : it->second.get();
}

// ---------- Listados ----------
void Store::list_products() const {
    if (catalog_.empty()) { std::cout << "No hay productos.\n"; return; }
    for (const auto& [id, p] : catalog_)
        if (p) p->showInfo();
}

void Store::list_clients() const {
    if (clients_.empty()) { std::cout << "No hay clientes.\n"; return; }
    for (const auto& [id, c] : clients_)
        if (c) c->showInfo();
}

Client* Store::find_client_by_name(const std::string& name) {
    std::vector<Client*> matches;
    for (auto& [_, c] : clients_) {
        if (c && c->getName() == name)
            matches.push_back(c.get());
    }

    if (matches.empty()) return nullptr;
    if (matches.size() == 1) return matches.front();

    // ⚠️ varios con el mismo nombre → pedir elección
    std::cout << "Hay " << matches.size() << " clientes llamados '" << name << "':\n";
    for (size_t i = 0; i < matches.size(); ++i)
        std::cout << "  [" << i+1 << "] id=" << matches[i]->getId() << "\n";

    int sel;
    std::cout << "Elegí número: ";
    std::cin >> sel;
    if (sel < 1 || sel > (int)matches.size()) return nullptr;
    return matches[sel-1];
}


Product* Store::find_product_by_name(const std::string& name) {
    const std::string key = to_lower(name);
    for (auto& [id, up] : catalog_) {
        if (!up) continue;
        if (to_lower(up->getName()) == key) return up.get();
    }
    return nullptr;
}

void Store::print_client_transactions(int clientId) const {
    JSON_DB txdb{Alias::Transactions};
    const auto& arr = txdb.all();
    if (!arr.is_array() || arr.empty()) {
        std::cout << "No hay transacciones registradas.\n";
        return;
    }

    bool found = false;
    for (const auto& t : arr) {
        // --- cliente.id puede ser string o número ---
        int cid = -1;
        const auto& jcid = t["client"]["id"];
        if (jcid.is_string())       cid = std::stoi(jcid.get<std::string>());
        else if (jcid.is_number())  cid = jcid.get<int>();
        else continue;

        if (cid != clientId) continue;
        found = true;

        std::string action = t.value("action", "?");
        int qty = t.value("qty", 0);

        // --- product.id puede ser string o número ---
        int pid = -1;
        const auto& jpid = t["product"]["id"];
        if (jpid.is_string())       pid = std::stoi(jpid.get<std::string>());
        else if (jpid.is_number())  pid = jpid.get<int>();

        std::string pname = t["product"].value("name", "(sin nombre)");

        std::cout << "- " << action
                  << "  " << qty << " x [" << pid << "] "
                  << pname << "\n";
    }

    if (!found)
        std::cout << "El cliente no tiene transacciones registradas.\n";
}
