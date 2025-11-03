#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <vector>
#include "headers/store.h"
#include "headers/transaction.h"

// ---- Entradas seguras ----
int readInt(const char* prompt) {
    int x;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada inválida.\n";
    }
}

float readFloat(const char* prompt) {
    float x;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada inválida.\n";
    }
}

std::string readStr(const char* prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin >> std::ws, s);
    return s;
}

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

int main() {
    Store store;
    store.loadFromDisk();            // ✅ nombre actualizado
    TransactionService tx(store);

    for (;;) {
        std::cout <<
          "\n=== MENU ===\n"
          "1) Ver TODOS los productos\n"
          "2) Ver SOLO videojuegos\n"
          "3) Ver SOLO películas\n"
          "4) Ver clientes\n"
          "5) Agregar videojuego\n"
          "6) Agregar película\n"
          "7) Agregar cliente\n"
          "8) Alquilar (por id o nombre)\n"
          "9) Devolver (por id o nombre)\n"
          "10) Ver historial de un cliente\n"
          "11) Filtrar videojuegos por género\n"
          "12) Filtrar películas por género\n"
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
                store.listProducts();   // ✅
                break;
            case 2:
                store.listGames();      // ✅
                break;
            case 3:
                store.listMovies();     // ✅
                break;
            case 4:
                store.listClients();    // ✅
                break;
            case 5: { // Alta juego
                auto name = readStr("name: ");
                auto gen  = readStr("genero: ");
                auto desc = readStr("description: ");
                float price = readFloat("price: ");
                int stock   = readInt("totalStock: ");
                auto plat   = readStr("platform: ");
                auto plays  = readStr("players: ");
                int id = store.addGame(name, gen, desc, price, stock, plat, plays); // ✅
                std::cout << (id >= 0 ? "Game agregado con ID " + std::to_string(id) + ".\n"
                                       : "Error al agregar.\n");
                break;
            }
            case 6: { // Alta película
                auto name = readStr("name: ");
                auto gen  = readStr("genero: ");
                auto desc = readStr("description: ");
                float price = readFloat("price: ");
                int stock   = readInt("totalStock: ");
                auto dir    = readStr("director: ");
                int dur     = readInt("durationMin: ");
                int id = store.addMovie(name, gen, desc, price, stock, dir, dur); // ✅
                std::cout << (id >= 0 ? "Movie agregada con ID " + std::to_string(id) + ".\n"
                                       : "Error al agregar.\n");
                break;
            }
            case 7: { // Alta cliente
                auto name = readStr("name: ");
                int id = store.addClient(name); // ✅
                std::cout << (id >= 0 ? "Cliente agregado con ID " + std::to_string(id) + ".\n"
                                       : "Error al agregar.\n");
                break;
            }
            case 8: { // Alquilar
                Client*  c = chooseClient(store);
                Product* p = chooseProduct(store);
                if (!c || !p) { std::cout << "No se pudo resolver cliente/producto.\n"; break; }
                int qty = readInt("Cantidad: ");
                std::cout << (tx.rent(*p, *c, qty) ? "Alquiler OK.\n" : "Alquiler rechazado.\n");
                break;
            }
            case 9: { // Devolver
                Client*  c = chooseClient(store);
                Product* p = chooseProduct(store);
                if (!c || !p) { std::cout << "No se pudo resolver cliente/producto.\n"; break; }
                int qty = readInt("Cantidad: ");
                std::cout << (tx.giveBack(*p, *c, qty) ? "Devolución OK.\n" : "Devolución rechazada.\n");
                break;
            }
            case 10: { // Historial cliente
                Client* c = chooseClient(store);
                if (!c) { std::cout << "Cliente no encontrado.\n"; break; }
                std::cout << "\nHistorial de " << c->getName()
                          << " (id=" << c->getId() << "):\n";
                store.printClientTransactions(c->getId()); // ✅
                break;
            }
            case 11: { // Filtrar juegos
                auto gen = readStr("Género (exacto): ");
                store.listGamesByGenre(gen); // ✅
                break;
            }
            case 12: { // Filtrar películas
                auto gen = readStr("Género (exacto): ");
                store.listMoviesByGenre(gen); // ✅
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
