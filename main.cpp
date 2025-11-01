#include <iostream>
#include <limits>
#include <algorithm>
#include <cctype>
#include "headers/store.h"
#include "headers/transaction.h"

// ---- Entradas seguras ----
int read_int(const char* prompt) {
    int x;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida.\n";
    }
}
float read_float(const char* prompt) {
    float x;
    for (;;) {
        std::cout << prompt;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida.\n";
    }
}
std::string read_str(const char* prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin >> std::ws, s);
    return s;
}

// ---- Resolver cliente/producto por ID o nombre ----
Client* resolve_client(Store& store) {
    std::string token = read_str("Cliente (id o nombre): ");
    bool isnum = !token.empty() && std::all_of(token.begin(), token.end(), ::isdigit);
    if (isnum) {
        int cid = std::stoi(token);
        return store.find_client(cid);
    }
    return store.find_client_by_name(token);
}
Product* resolve_product(Store& store) {
    std::string token = read_str("Producto (id o nombre): ");
    bool isnum = !token.empty() && std::all_of(token.begin(), token.end(), ::isdigit);
    if (isnum) {
        int pid = std::stoi(token);
        return store.find_product(pid);
    }
    return store.find_product_by_name(token);
}

int main() {
    // 1) Inicializar servicios
    Store store;                 // dueño del catálogo en memoria + persistencia
    store.load_from_disk();      // levanta games/movies/clients desde JSON
    TransactionService tx(store);// tx usará store para snapshot de productos

    // 2) Menú de texto
    for (;;) {
        std::cout <<
          "\n=== MENU ===\n"
          "1) Ver productos\n"
          "2) Ver clientes (con nombres de alquileres)\n"
          "3) Agregar videojuego\n"
          "4) Agregar pelicula\n"
          "5) Agregar cliente\n"
          "6) Alquilar (por id o nombre)\n"
          "7) Devolver (por id o nombre)\n"
          "8) Ver historial de un cliente\n"
          "0) Salir (guarda catalogo)\n";
        int op = read_int("Opcion: ");
        if (op == 0) {
            store.save_to_disk(); // snapshot final del catálogo (productos)
            std::cout << "Adios!\n";
            break;
        }

        switch (op) {
        case 1: {
            store.list_products();
            break;
        }
        case 2: {
            // Muestra TODOS los clientes; si tienen alquileres, imprime nombres y qty
            store.list_clients();
            break;
        }
        case 3: { // agregar game
            int id = read_int("id: ");
            auto name = read_str("name: ");
            auto gen  = read_str("genero: ");
            auto desc = read_str("description: ");
            float price = read_float("price: ");
            int stock   = read_int("totalStock: ");
            auto plat   = read_str("platform: ");
            auto plays  = read_str("players: ");
            bool ok = store.add_game(id, name, gen, desc, price, stock, plat, plays);
            std::cout << (ok ? "Game agregado.\n" : "Error (id duplicado o persistencia).\n");
            break;
        }
        case 4: { // agregar movie
            int id = read_int("id: ");
            auto name = read_str("name: ");
            auto gen  = read_str("genero: ");
            auto desc = read_str("description: ");
            float price = read_float("price: ");
            int stock   = read_int("totalStock: ");
            auto dir    = read_str("director: ");
            int dur     = read_int("durationMin: ");
            bool ok = store.add_movie(id, name, gen, desc, price, stock, dir, dur);
            std::cout << (ok ? "Movie agregada.\n" : "Error (id duplicado o persistencia).\n");
            break;
        }
        case 5: { // agregar cliente
            int id = read_int("id: ");
            auto name = read_str("name: ");
            bool ok = store.add_client(id, name);
            std::cout << (ok ? "Cliente agregado.\n" : "Error (id duplicado o persistencia).\n");
            break;
        }
        case 6: { // alquilar (por id o nombre)
            Client*  c = resolve_client(store);
            Product* p = resolve_product(store);
            if (!c || !p) { std::cout << "Cliente o producto no encontrado.\n"; break; }
            int qty = read_int("Cantidad: ");
            std::cout << (tx.rent(*p, *c, qty) ? "Alquiler OK.\n" : "Alquiler rechazado.\n");
            break;
        }
        case 7: { // devolver (por id o nombre)
            Client*  c = resolve_client(store);
            Product* p = resolve_product(store);
            if (!c || !p) { std::cout << "Cliente o producto no encontrado.\n"; break; }
            int qty = read_int("Cantidad: ");
            std::cout << (tx.giveBack(*p, *c, qty) ? "Devolucion OK.\n" : "Devolucion rechazada.\n");
            break;
        }
        case 8: { // historial
            Client* c = resolve_client(store);
            if (!c) { std::cout << "Cliente no encontrado.\n"; break; }
            std::cout << "\nHistorial de " << c->getName()
                      << " (id=" << c->getId() << "):\n";
            store.print_client_transactions(c->getId());
            break;
        }
        default:
            std::cout << "Opcion invalida.\n";
        }
    }

    return 0;
}
