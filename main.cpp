#include <iostream>
#include <limits>
#include "headers/store.h"
#include "headers/transaction.h"

// Entradas seguras básicas
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
          "2) Ver clientes\n"
          "3) Agregar videojuego\n"
          "4) Agregar pelicula\n"
          "5) Agregar cliente\n"
          "6) Alquilar\n"
          "7) Devolver\n"
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
        case 6: { // alquilar
            int cid = read_int("ID cliente: ");
            int pid = read_int("ID producto: ");
            int qty = read_int("Cantidad: ");
            Client*  c = store.find_client(cid);
            Product* p = store.find_product(pid);
            if (!c || !p) { std::cout << "Cliente o producto inexistente.\n"; break; }
            std::cout << (tx.rent(*p, *c, qty) ? "Alquiler OK.\n" : "Alquiler rechazado.\n");
            break;
        }
        case 7: { // devolver
            int cid = read_int("ID cliente: ");
            int pid = read_int("ID producto: ");
            int qty = read_int("Cantidad: ");
            Client*  c = store.find_client(cid);
            Product* p = store.find_product(pid);
            if (!c || !p) { std::cout << "Cliente o producto inexistente.\n"; break; }
            std::cout << (tx.giveBack(*p, *c, qty) ? "Devolucion OK.\n" : "Devolucion rechazada.\n");
            break;
        }
        default:
            std::cout << "Opcion invalida.\n";
        }
    }

    return 0;
}
