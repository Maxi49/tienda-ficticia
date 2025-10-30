#include <iostream>
#include "headers/client.h"
#include "headers/products/game.h"
#include "headers/products/movie.h"
#include "headers/transaction.h"
#include "headers/store.h"   // solo si querés guardar catálogo manualmente (opcional)

int main() {
    std::cout << "=== PRUEBA INTEGRAL (Client + Product + Store + Transaction) ===\n";

    // 1) Crear clientes
    Client c1(1, "Maxi");
    Client c2(2, "Ana");

    // (Opcional) Persistir alta de clientes aunque no transaccionen aún:
    Client::upsert(c1);
    Client::upsert(c2);

    // 2) Crear productos
    Game  g1(101, "God of War", "Accion", "Juego de PS4", 59.99f, 5, "PS4", "1 jugador");
    Movie m1(201, "Interstellar", "Sci-Fi", "Exploración espacial", 39.99f, 3, "Christopher Nolan", 169);

    // (Opcional) Guardar el catálogo inicial sin transaccionar:
    // Store store;
    // store.upsert(g1);   // -> db/games.json
    // store.upsert(m1);   // -> db/movies.json

    // 3) Mostrar estado inicial
    std::cout << "\n--- Estado inicial ---\n";
    g1.showInfo();
    m1.showInfo();
    c1.showInfo();
    c2.showInfo();

    // 4) Transacciones
    TransactionService tx;

    std::cout << "\n--- Alquiler 1: Maxi alquila 2 copias de God of War ---\n";
    if (tx.rent(g1, c1, 2))  std::cout << "✅ Rent OK\n"; else std::cout << "❌ Rent FAIL\n";

    std::cout << "\n--- Alquiler 2: Ana alquila 1 copia de Interstellar ---\n";
    if (tx.rent(m1, c2, 1))  std::cout << "✅ Rent OK\n"; else std::cout << "❌ Rent FAIL\n";

    std::cout << "\n--- Estado luego de alquilar ---\n";
    g1.showInfo();
    m1.showInfo();
    c1.showInfo();
    c2.showInfo();

    std::cout << "\n--- Devolución: Maxi devuelve 1 copia de God of War ---\n";
    if (tx.giveBack(g1, c1, 1)) std::cout << "✅ Return OK\n"; else std::cout << "❌ Return FAIL\n";

    std::cout << "\n--- Estado final ---\n";
    g1.showInfo();
    m1.showInfo();
    c1.showInfo();
    c2.showInfo();

    std::cout << "\nRevisá los archivos JSON actualizados en /db/ :\n"
                 " - clients.json (snapshot de clientes)\n"
                 " - games.json   (snapshot de games)\n"
                 " - movies.json  (snapshot de movies)\n"
                 " - transactions.json (historial de acciones)\n";

    return 0;
}
