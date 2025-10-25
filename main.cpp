#include <iostream>
#include "headers/store.h"

int main() {
    Store tienda;

    while (true) {
        std::cout << "\n==== TIENDA - MENU ====" << std::endl;
        std::cout << "1) Crear cliente" << std::endl;
        std::cout << "2) Crear juego" << std::endl;
        std::cout << "3) Crear pelicula" << std::endl;
        std::cout << "4) Listar clientes" << std::endl;
        std::cout << "5) Listar productos" << std::endl;
        std::cout << "6) Alquilar producto" << std::endl;
        std::cout << "7) Devolver producto" << std::endl;
        std::cout << "8) Mostrar cliente por id" << std::endl;
        std::cout << "9) Mostrar producto por id" << std::endl;
        std::cout << "0) Salir" << std::endl;

        int opt;
        std::cout << "Seleccioná una opción: ";
        if (!(std::cin >> opt)) break;

        if (opt == 0) break;

        if (opt == 1) {
            std::string name;
            std::cout << "Nombre cliente: ";
            std::cin.ignore();
            std::getline(std::cin, name);
            int id = tienda.createClient(name);
            std::cout << "Cliente creado con id: " << id << std::endl;
        } else if (opt == 2) {
            std::cin.ignore();
            std::string name, genero, desc, platform, players;
            float price; int stock;
            std::cout << "Nombre juego: "; std::getline(std::cin, name);
            std::cout << "Genero: "; std::getline(std::cin, genero);
            std::cout << "Descripcion: "; std::getline(std::cin, desc);
            std::cout << "Precio: "; std::cin >> price;
            std::cout << "Stock total: "; std::cin >> stock;
            std::cin.ignore();
            std::cout << "Plataforma: "; std::getline(std::cin, platform);
            std::cout << "Jugadores: "; std::getline(std::cin, players);
            int id = tienda.createGame(name, genero, desc, price, stock, platform, players);
            std::cout << "Juego creado con id: " << id << std::endl;
        } else if (opt == 3) {
            std::cin.ignore();
            std::string name, genero, desc, director;
            float price; int stock, duration;
            std::cout << "Nombre pelicula: "; std::getline(std::cin, name);
            std::cout << "Genero: "; std::getline(std::cin, genero);
            std::cout << "Descripcion: "; std::getline(std::cin, desc);
            std::cout << "Precio: "; std::cin >> price;
            std::cout << "Stock total: "; std::cin >> stock;
            std::cout << "Duracion (min): "; std::cin >> duration;
            std::cin.ignore();
            std::cout << "Director: "; std::getline(std::cin, director);
            int id = tienda.createMovie(name, genero, desc, price, stock, director, duration);
            std::cout << "Pelicula creada con id: " << id << std::endl;
        } else if (opt == 4) {
            tienda.listClients();
        } else if (opt == 5) {
            tienda.listProducts();
        } else if (opt == 6) {
            int pid, cid, qty;
            std::cout << "Id producto: "; std::cin >> pid;
            std::cout << "Id cliente: "; std::cin >> cid;
            std::cout << "Cantidad: "; std::cin >> qty;
            if (tienda.rentProduct(pid, cid, qty))
                std::cout << "Alquiler realizado correctamente." << std::endl;
            else
                std::cout << "Error al realizar alquiler." << std::endl;
        } else if (opt == 7) {
            int pid, cid, qty;
            std::cout << "Id producto: "; std::cin >> pid;
            std::cout << "Id cliente: "; std::cin >> cid;
            std::cout << "Cantidad: "; std::cin >> qty;
            if (tienda.returnProduct(pid, cid, qty))
                std::cout << "Devolucion realizada correctamente." << std::endl;
            else
                std::cout << "Error al realizar devolucion." << std::endl;
        } else if (opt == 8) {
            int cid; std::cout << "Id cliente: "; std::cin >> cid;
            tienda.showClientInfo(cid);
        } else if (opt == 9) {
            int pid; std::cout << "Id producto: "; std::cin >> pid;
            tienda.showProductInfo(pid);
        } else {
            std::cout << "Opción no válida." << std::endl;
        }
    }

    std::cout << "Saliendo..." << std::endl;
    return 0;
}