#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include "headers/store.h"
#include "headers/transaction.h"
#include "read_inputs.h"
#include "templates.h"

// ---- Resolver cliente/producto (por ID o nombre) ----
Client* chooseClient(Store& store) {
    std::string token = readStr("Cliente (id o nombre): ");
    bool isNum = !token.empty() && std::all_of(token.begin(), token.end(), ::isdigit);
    if (isNum) return store.findClient(std::stoi(token));

    auto matches = store.findClientsByName(token, true);
    if (matches.empty()) { std::cout << "Sin coincidencias.\n"; return nullptr; }
    if (matches.size() == 1) return matches[0];

    std::cout << "Coincidencias:\n";
    for (size_t i = 0; i < matches.size(); ++i) {
        std::cout << " [" << i + 1 << "] ID " << matches[i]->getId()
                  << "  Nombre " << matches[i]->getName() << "\n";
    }
    int sel = readInt("Elegir #: ");
    if (sel < 1 || sel > (int)matches.size()) return nullptr;
    return matches[sel - 1];
}

Product* chooseProduct(Store& store) {
    std::string token = readStr("Producto (id o nombre): ");
    bool isNum = !token.empty() && std::all_of(token.begin(), token.end(), ::isdigit);
    if (isNum) return store.findProduct(std::stoi(token));

    auto matches = store.findProductsByName(token, true);
    if (matches.empty()) { std::cout << "Sin coincidencias.\n"; return nullptr; }
    if (matches.size() == 1) return matches[0];

    std::cout << "Coincidencias:\n";
    for (size_t i = 0; i < matches.size(); ++i) {
        std::cout << " [" << i + 1 << "] ID " << matches[i]->getId()
                  << "  Nombre " << matches[i]->getName()
                  << "  Tipo " << matches[i]->type() << "\n";
    }
    int sel = readInt("Elegir #: ");
    if (sel < 1 || sel > (int)matches.size()) return nullptr;
    return matches[sel - 1];
}


struct MenuItem {
    int key;
    std::string label;
    std::function<void()> action;
};

inline void print_menu(const std::string& title,
                       const std::vector<MenuItem>& items,
                       int exitKey,
                       const std::string& exitLabel)
{
    std::cout << "\n=== " << title << " ===\n";
    for (const auto& it : items) {
        std::cout << it.key << ") " << it.label << '\n';
    }
    std::cout << exitKey << ") " << exitLabel << '\n';
}

// corre un loop hasta que el user elija exitKey
inline void run_menu_loop(const std::string& title,
                          std::vector<MenuItem> items,
                          int exitKey = 0,
                          std::string exitLabel = "Salir")
{
    for (;;) {
        print_menu(title, items, exitKey, exitLabel);
        int op = readInt("Opción: ");

        if (op == exitKey) break;

        bool dispatched = false;
        for (auto& it : items) {
            if (it.key == op) {
                try {
                    it.action();
                } catch (const std::exception& e) {
                    std::cerr << "[menu:" << title << "] Error: " << e.what() << "\n";
                }
                dispatched = true;
                break;
            }
        }
        if (!dispatched) std::cout << "⚠️ Opción inválida.\n";
    }
}

void administrate_games(Store& store) {
    run_menu_loop("Administrar Juegos", {
        {1, "Mostrar juegos", [&]{ store.listGames(); }},
        {2, "Añadir juego",   [&]{
            GameInput g = store.readGameInput();
            auto& [name, genre, description, platform, players, price, stock] = g;
            const int id = store.addGame(name, genre, description, price, stock, platform, players);
            std::cout << (id >= 0
                ? "✅ Juego agregado con ID " + std::to_string(id) + ".\n"
                : "❌ Error al agregar.\n");
        }},
        {3, "Buscar juego (por nombre)", [&]{
            const auto name = readStr("Nombre a buscar: ");
            const std::vector<Product*> out = store.findProductsByName(name);
            showVectorInfo(out);
        }},
        {4, "Filtrar juego por género (exacto)", [&]{
            const auto gen = readStr("Género (exacto): ");
            store.listGamesByGenre(gen);
        }},
    });
}

void administrate_clients(Store& store) {
    run_menu_loop("Administrar Clientes", {
        {1, "Mostrar clientes", [&]{ store.listClients(); }},
        {2, "Crear cliente",    [&]{
            ClientInput c = store.readClientInput();
            auto& [name] = c;
            const int id = store.addClient(name);
            std::cout << (id >= 0
                ? "✅ Cliente creado con ID " + std::to_string(id) + ".\n"
                : "❌ Error al crear cliente.\n");
        }},
        {3, "Ver historial de un cliente", [&]{
            Client* c = chooseClient(store);
            if (!c) { std::cout << "Cliente no encontrado.\n"; return; }
            std::cout << "\nHistorial de " << c->getName()
                      << " (id=" << c->getId() << "):\n";
            store.printClientTransactions(c->getId());
        }},
    });
}

void administrate_movies(Store& store) {
    run_menu_loop("Administrar Películas", {
        {1, "Mostrar películas", [&]{ store.listMovies(); }},
        {2, "Añadir película",   [&]{
            MovieInput m = store.readMovieInput();
            auto& [name, genre, description, director, price, stock, duration] = m;
            const int id = store.addMovie(name, genre, description, price, stock, director, duration);
            std::cout << (id >= 0
                ? "✅ Película agregada con ID " + std::to_string(id) + ".\n"
                : "❌ Error al agregar.\n");
        }},
        {3, "Buscar película (por nombre)", [&]{
            const auto name = readStr("Nombre a buscar: ");
            const std::vector<Product*> out = store.findProductsByName(name);
            showVectorInfo(out);
        }},
        {4, "Filtrar película por género (exacto)", [&]{
            const auto gen = readStr("Género (exacto): ");
            store.listMoviesByGenre(gen);
        }},
    });
}

void administrate_products(Store& store, TransactionService& tx) {
    run_menu_loop("Administrar Productos", {
        {1, "Listar todos los productos", [&]{ store.listProducts(); }},
        {2, "Buscar producto (seleccionar)", [&]{
            const Product* p = chooseProduct(store);
            std::cout << p << '\n';
        }},
        {3, "Alquilar producto", [&]{
            Client*  c = chooseClient(store);
            Product* p = chooseProduct(store);
            if (!c || !p) { std::cout << "No se pudo resolver cliente/producto.\n"; return; }
            int qty = readInt("Cantidad: ");
            std::cout << (tx.rent(*p, *c, qty) ? "Alquiler OK.\n" : "Alquiler rechazado.\n");
        }},
        {4, "Devolver producto", [&]{
            Client*  c = chooseClient(store);
            Product* p = chooseProduct(store);
            if (!c || !p) { std::cout << "No se pudo resolver cliente/producto.\n"; return; }
            int qty = readInt("Cantidad: ");
            std::cout << (tx.giveBack(*p, *c, qty) ? "Devolución OK.\n" : "Devolución rechazada.\n");
        }},
    });
}

int main() {
    Store store;
    store.loadFromDisk();
    TransactionService tx(store);

    run_menu_loop("MENU PRINCIPAL", {
        {1, "Administrar Productos",  [&]{ administrate_products(store, tx); }},
        {2, "Administrar Películas",  [&]{ administrate_movies(store); }},
        {3, "Administrar Juegos",     [&]{ administrate_games(store); }},
        {4, "Administrar Clientes",   [&]{ administrate_clients(store); }},
    }, /*exitKey*/0, /*exitLabel*/"Salir (guardar)");

    try {
        store.saveToDisk();
    } catch (const std::exception& e) {
        std::cerr << "[save] Error: " << e.what() << "\n";
    }
    std::cout << "Adiós!\n";
    return 0;
}
