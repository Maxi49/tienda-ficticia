#include <iostream>
#include "headers/products/products.h"
#include "single_include/nlohmann/json.hpp"
#include "headers/client.h"
#include "headers/transaction.h"

using json = nlohmann::json;
#include "../headers/products/game.h"

int main() {
    TransactionService tx;

    Client cliente1(1, "Maxi");
    Client cliente2(2, "Ana");

    // Guardar “al crear”, sin transacción:
    tx.registerClient(cliente1);
    tx.registerClient(cliente2);

    // … y luego, lo de siempre:
    Game juego1(101, "God of War", "Accion", "Juego de PS4", 59.99f, 5, "PlayStation 4", "1 jugador");

    tx.rent(juego1, cliente1, 2);
    tx.giveBack(juego1, cliente1, 1);

    return 0;
}
