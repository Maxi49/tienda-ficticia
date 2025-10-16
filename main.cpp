#include <iostream>
#include "headers/products/products.h"
#include "single_include/nlohmann/json.hpp"
#include "headers/client.h"
#include "headers/transaction.h"

using json = nlohmann::json;
#include "../headers/products/game.h"

int main() {
    std::cout << "=== TEST DE TRANSACCIONES CON GAME ===" << std::endl;

    // 1️⃣ Crear un cliente
    Client cliente1(1, "Maxi");

    // 2️⃣ Crear un videojuego (Game)
    Game juego1(
        101,                   // id
        "God of War",          // nombre
        "Accion",              // genero
        "Juego de PS4",        // descripcion
        59.99f,                // precio
        5,                     // stock total
        "PlayStation 4",       // plataforma
        "1 jugador"            // jugadores
    );

    // 3️⃣ Mostrar info inicial
    std::cout << "\n--- Estado inicial ---" << std::endl;
    juego1.showInfo();
    cliente1.showInfo();

    // 4️⃣ Crear servicio de transacciones
    TransactionService tx;

    // 5️⃣ Realizar un alquiler
    std::cout << "\n--- Alquilando 2 unidades ---" << std::endl;
    if (tx.rent(juego1, cliente1, 2)) {
        std::cout << "✅ Alquiler registrado correctamente." << std::endl;
    } else {
        std::cout << "❌ Error al registrar el alquiler." << std::endl;
    }

    // 6️⃣ Mostrar estado después del alquiler
    std::cout << "\n--- Después del alquiler ---" << std::endl;
    juego1.showInfo();
    cliente1.showInfo();

    // 7️⃣ Realizar una devolución
    std::cout << "\n--- Devolviendo 1 unidad ---" << std::endl;
    if (tx.giveBack(juego1, cliente1, 1)) {
        std::cout << "✅ Devolución registrada correctamente." << std::endl;
    } else {
        std::cout << "❌ Error al registrar la devolución." << std::endl;
    }

    // 8️⃣ Mostrar estado final
    std::cout << "\n--- Estado final ---" << std::endl;
    juego1.showInfo();
    cliente1.showInfo();

    std::cout << "\n📁 Revisá el archivo /db/transactions.json para ver los registros guardados.\n";

    return 0;
}