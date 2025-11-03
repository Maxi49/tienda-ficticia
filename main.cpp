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

void administrate_games(Store& store) {
    std::cout << "1) Mostrar juegos" << std::endl;
    std::cout << "2) Añadir  juego" << std::endl;
    std::cout << "3) Buscar juego" << std::endl;
    std::cout << "0) Salir" << std::endl;
    const int option = readInt("Operation: ");


    std::map<int, std::function<void()>> actions;

    actions[1] = [&]() {
        store.listGames();
    };

    actions[2] = [&]() {
        GameInput g = store.readGameInput();

        auto& [name, genre, description, platform, players, price, stock] = g;

        const int id = store.addGame(name, genre, description, price, stock, platform, players);
        std::cout << (id >= 0
            ? "✅ Juego agregada con ID " + std::to_string(id) + ".\n"
            : "❌ Error al agregar.\n");
    };

    actions[3] = [&]() {
        const auto name = readStr("Nombre a buscar: ");
        const std::vector<Product *> out = store.findProductsByName(name);
        showVectorInfo(out);
    };

    actions[4] = [&]() {
        const auto gen = readStr("Género (exacto): ");
        store.listGamesByGenre(gen);
    };

    actions[0] = [&]() {};

    if (const auto it = actions.find(option); it != actions.end()) {
        it->second();  // ejecuta la acción correspondiente
    } else {
        std::cout << "⚠️ Opción inválida.\n";
    }
}

void administrate_clients(Store& store) {
    std::cout << "1) Mostrar Clientes" << std::endl;
    std::cout << "2) Crear Cliente" << std::endl;
    std::cout << "3) Ver historial del cliente" << std::endl;
    std::cout << "0) Salir" << std::endl;
    const int option = readInt("Operation: ");


    std::map<int, std::function<void()>> actions;

    actions[1] = [&]() {
        store.listClients();
    };

    actions[2] = [&]() {
        ClientInput c = store.readClientInput();

        auto& [name] = c;

        const int id = store.addClient(name);
        std::cout << (id >= 0
            ? "✅ Movie agregada con ID " + std::to_string(id) + ".\n"
            : "❌ Error al agregar.\n");
    };


    actions[3] = [&]() {
        Client* c = chooseClient(store);
        if (!c) { std::cout << "Cliente no encontrado.\n"; }
        std::cout << "\nHistorial de " << c->getName()
                  << " (id=" << c->getId() << "):\n";
        store.printClientTransactions(c->getId()); // ✅
    };

    actions[0] = [&]() {};

    if (const auto it = actions.find(option); it != actions.end()) {
        it->second();  // ejecuta la acción correspondiente
    } else {
        std::cout << "⚠️ Opción inválida.\n";
    }

}

void administrate_movies(Store& store) {
    std::cout << "1) Mostrar peliculas" << std::endl;
    std::cout << "2) Añadir  pelicula" << std::endl;
    std::cout << "3) Buscar pelicula" << std::endl;
    std::cout << "3) Filtrar pelicula por genero" << std::endl;
    std::cout << "0) Salir" << std::endl;
    const int option = readInt("Operation: ");


    std::map<int, std::function<void()>> actions;

    actions[1] = [&]() {
        store.listMovies();
    };

    actions[2] = [&]() {
        MovieInput m = store.readMovieInput();

        auto& [name, genre, description, director, price, stock, duration] = m;

        const int id = store.addMovie(name, genre, description, price, stock, director, duration);
        std::cout << (id >= 0
            ? "✅ Movie agregada con ID " + std::to_string(id) + ".\n"
            : "❌ Error al agregar.\n");
    };

    actions[3] = [&]() {
        const auto name = readStr("Nombre a buscar: ");
        const std::vector<Product *> out = store.findProductsByName(name);
        showVectorInfo(out);
    };

    actions[4] = [&]() {
        const auto gen = readStr("Género (exacto): ");
        store.listMoviesByGenre(gen); // ✅
    };

    actions[0] = [&]() {};

    if (const auto it = actions.find(option); it != actions.end()) {
        it->second();  // ejecuta la acción correspondiente
    } else {
        std::cout << "⚠️ Opción inválida.\n";
    }
}


void administrate_products(Store& store, TransactionService& tx) {
    std::cout << "1) Listar todos los productos" << std::endl;
    std::cout << "2) Buscar producto" << std::endl;
    std::cout << "3) Alquilar producto" << std::endl;
    std::cout << "4) Devolver producto" << std::endl;
    std::cout << "0) Salir" << std::endl;

    const int option = readInt("Operation: ");

    std::map<int, std::function<void()>> actions;

    actions[1] = [&]() {
        store.listProducts();
    };

    actions[2] = [&] () {
        const Product* result = chooseProduct(store);

        std::cout << result << std::endl;
    };

    actions[3] = [&]() {
        Client*  c = chooseClient(store);
        Product* p = chooseProduct(store);
        if (!c || !p) {
            std::cout << "No se pudo resolver cliente/producto.\n" << std::endl;
            return;
        };

        int qty = readInt("Cantidad: ");
        std::cout << (tx.rent(*p, *c, qty) ? "Alquiler OK.\n" : "Alquiler rechazado.\n");
    };

    actions[4] = [&]() {
        Client*  c = chooseClient(store);
        Product* p = chooseProduct(store);
        if (!c || !p) {
            std::cout << "No se pudo resolver cliente/producto.\n";
            return;
        }
        int qty = readInt("Cantidad: ");
        std::cout << (tx.giveBack(*p, *c, qty) ? "Devolución OK.\n" : "Devolución rechazada.\n");
    };

    actions[0] = [&]() {};

    if (const auto it = actions.find(option); it != actions.end()) {
        it->second();  // ejecuta la acción correspondiente
    } else {
        std::cout << "⚠️ Opción inválida.\n";
    }
}


int main() {
    Store store;
    store.loadFromDisk();            // ✅ nombre actualizado
    TransactionService tx(store);

    /*
     * ADMINISTRAR PRODUCTOS
     * ADMINISTRAR PELICULAS
     * ADMINISTRAR JUEGOS
     * ADMINISTRAR CLIENTES
     */

    for (;;) {
        std::cout <<
          "\n=== MENU ===\n"
          "1) Administrar Productos \n"
          "2) Administrar Peliculas \n"
          "3) Administrar Juegos \n"
          "4) Administrar Clientes \n"
          "0) Salir (guardar)\n";

        int op = readInt("Opción: ");
        if (op == 0) {
            try {
                store.saveToDisk();   // ✅ nombre actualizado
            } catch (const std::exception& e) {
                std::cerr << "[save] Error: " << e.what() << "\n";
            }
            std::cout << "Adiós!\n";
            break;
        }

        try {
            switch (op) {
            case 1:
                administrate_products(store, tx);
                break;
            case 2: { // Alta juego
                administrate_movies(store);
                break;
            }
            case 3: { // Alta cliente
                administrate_games(store);
                break;
            }
            case 4: { // Alta cliente
                administrate_clients(store);
                break;
            }
            default:
                std::cout << "Opción inválida.\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "[menu] Error: " << e.what() << "\n";
        }
    }

    return 0;
}
