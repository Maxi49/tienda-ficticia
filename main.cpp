#include <iostream>
#include <algorithm>
#include <cctype>
#include <vector>
#include "headers/store.h"
#include "headers/transaction.h"
#include "utils/read_inputs.h"
#include "utils/templates.h"

/**
 * @brief Selecciona un cliente por ID numérico o por nombre (con coincidencia parcial).
 *
 * Flujo:
 *  - Pide un token de entrada. Si el token es numérico => busca por ID.
 *  - Si no es numérico => busca por nombre con coincidencia parcial (case-insensitive).
 *  - Si hay múltiples coincidencias, lista y solicita una selección explícita.
 *
 * @param store Referencia a la tienda con el índice en memoria.
 * @return Client* Puntero prestado al cliente elegido, o nullptr si no hay coincidencias o la selección es inválida.
 *
 * @note El puntero devuelto es propiedad de Store; no debe liberarse ni almacenarse más allá del contexto inmediato.
 * @warning Si el usuario ingresa una opción fuera de rango, la función devuelve nullptr.
 */
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

/**
 * @brief Selecciona un producto por ID numérico o por nombre (con coincidencia parcial).
 *
 * Flujo:
 *  - Pide un token. Si es numérico => busca por ID.
 *  - Si no, realiza búsqueda parcial por nombre (case-insensitive).
 *  - Desambigua mostrando un listado cuando hay más de una coincidencia.
 *
 * @param store Referencia a la tienda con el catálogo en memoria.
 * @return Product* Puntero prestado al producto elegido, o nullptr si no hay coincidencias o la selección es inválida.
 *
 * @note El puntero devuelto es propiedad de Store; no debe ser liberado manualmente.
 * @warning Si el usuario elige un índice fuera de rango, se devuelve nullptr.
 */
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

/**
 * @brief Ítem de menú declarativo: clave numérica, etiqueta y acción asociada.
 *
 * @field key    Clave que el usuario ingresa para disparar la acción.
 * @field label  Texto descriptivo mostrado en el menú.
 * @field action Acción a ejecutar cuando se selecciona la clave.
 */
struct MenuItem {
    int key;
    std::string label;
    std::function<void()> action;
};

/**
 * @brief Imprime un menú encabezado y su lista de opciones.
 *
 * @param title     Título del menú (se muestra como encabezado).
 * @param items     Colección de ítems (clave + etiqueta) a listar.
 * @param exitKey   Clave numérica para salir del menú.
 * @param exitLabel Etiqueta a mostrar para la opción de salida.
 *
 * @note Solo imprime; no realiza lectura ni ejecución de acciones.
 */
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

/**
 * @brief Bucle interactivo de un menú: imprime, lee la opción y despacha acciones.
 *
 * Comportamiento:
 *  - Muestra el menú con @ref print_menu.
 *  - Lee una opción con `readInt("Opción: ")`.
 *  - Si coincide con @p exitKey, sale del bucle.
 *  - Si coincide con un @ref MenuItem::key, ejecuta su @ref MenuItem::action().
 *  - Maneja excepciones de actions, reportando en STDERR sin terminar el menú.
 *
 * @param title     Título del menú.
 * @param items     Ítems disponibles con sus acciones.
 * @param exitKey   Clave para abandonar el menú (por defecto 0).
 * @param exitLabel Etiqueta de la opción de salida (por defecto "Salir").
 *
 * @warning La búsqueda de ítems es lineal por simplicidad, adecuada para menús cortos.
 *          Si el menú creciera mucho, considerar un índice auxiliar por clave.
 * @note Requiere que las acciones sean no bloqueantes o que manejen internamente sus propios sub-bucles.
 */
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
        if (!dispatched) std::cout << " Opcion inválida.\n";
    }
}

/**
 * @brief Submenú de administración de videojuegos.
 *
 * Opciones:
 *  1. Listar juegos.
 *  2. Agregar juego (solicita datos via Store::readGameInput).
 *  3. Buscar por nombre (parcial) y mostrar coincidencias.
 *  4. Filtrar por género exacto.
 *
 * @param store Tienda a operar (lectura y altas).
 *
 * @post En caso de alta exitosa, el nuevo juego queda persistido y visible en listados.
 * @throws Propaga excepciones internas únicamente dentro de cada acción; el menú las captura.
 */
void administrate_games(Store& store) {
    run_menu_loop("Administrar Juegos", {
        {1, "Mostrar juegos", [&]{ store.listGames(); }},
        {2, "Añadir juego",   [&]{
            GameInput g = store.readGameInput();
            auto& [name, genre, description, platform, players, price, stock] = g;
            const int id = store.addGame(name, genre, description, price, stock, platform, players);
            std::cout << (id >= 0
                ? "Juego agregado con ID " + std::to_string(id) + ".\n"
                : " Error al agregar.\n");
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

/**
 * @brief Submenú de administración de clientes.
 *
 * Opciones:
 *  1. Listar clientes.
 *  2. Crear cliente (alta con nombre).
 *  3. Ver historial transaccional de un cliente (selección por ID o nombre).
 *
 * @param store Tienda con acceso a clientes y transacciones.
 *
 * @note `printClientTransactions` filtra sobre la base de transacciones y muestra solo del cliente elegido.
 */
void administrate_clients(Store& store) {
    run_menu_loop("Administrar Clientes", {
        {1, "Mostrar clientes", [&]{ store.listClients(); }},
        {2, "Crear cliente",    [&]{
            ClientInput c = store.readClientInput();
            auto& [name] = c;
            const int id = store.addClient(name);
            std::cout << (id >= 0
                ? "Cliente creado con ID " + std::to_string(id) + ".\n"
                : " Error al crear cliente.\n");
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

/**
 * @brief Submenú de administración de películas.
 *
 * Opciones:
 *  1. Listar películas.
 *  2. Agregar película (solicita datos via Store::readMovieInput).
 *  3. Buscar por nombre (parcial) y mostrar coincidencias.
 *  4. Filtrar por género exacto.
 *
 * @param store Tienda a operar (lectura y altas).
 */
void administrate_movies(Store& store) {
    run_menu_loop("Administrar Peliculas", {
        {1, "Mostrar peliculas", [&]{ store.listMovies(); }},
        {2, "Añadir peliculas",   [&]{
            MovieInput m = store.readMovieInput();
            auto& [name, genre, description, director, price, stock, duration] = m;
            const int id = store.addMovie(name, genre, description, price, stock, director, duration);
            std::cout << (id >= 0
                ? "Pelicula agregada con ID " + std::to_string(id) + ".\n"
                : "Error al agregar.\n");
        }},
        {3, "Buscar pelicula (por nombre)", [&]{
            const auto name = readStr("Nombre a buscar: ");
            const std::vector<Product*> out = store.findProductsByName(name);
            showVectorInfo(out);
        }},
        {4, "Filtrar pelicula por genero (exacto)", [&]{
            const auto gen = readStr("Genero (exacto): ");
            store.listMoviesByGenre(gen);
        }},
    });
}

/**
 * @brief Submenú de operaciones sobre productos (agnóstico al tipo).
 *
 * Opciones:
 *  1. Listar todos los productos del catálogo.
 *  2. Buscar producto y mostrar el puntero (útil para depuración/selección).
 *  3. Alquilar un producto a un cliente (valida cliente/producto y cantidad).
 *  4. Devolver un producto (valida cliente/producto y cantidad).
 *  5. Actualizar el precio de un producto (persiste y refleja en memoria).
 *
 * @param store Tienda con catálogo y utilidades de búsqueda/persistencia.
 * @param tx    Servicio de transacciones para rentas/devoluciones.
 *
 * @note La opción 5 delega en Store::updateProductPrice, que persiste vía JSON_DB y reconstruye el objeto en memoria.
 * @warning Las acciones 3/4 requieren que el stock y reglas de negocio estén validadas dentro de TransactionService.
 */
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
            int qty = readValue<int>("Cantidad: ");
            std::cout << (tx.rent(*p, *c, qty) ? "Alquiler OK.\n" : "Alquiler rechazado.\n");
        }},
        {4, "Devolver producto", [&]{
            Client*  c = chooseClient(store);
            Product* p = chooseProduct(store);
            if (!c || !p) { std::cout << "No se pudo resolver cliente/producto.\n"; return; }
            int qty = readValue<int>("Cantidad: ");
            std::cout << (tx.giveBack(*p, *c, qty) ? "Devolución OK.\n" : "Devolución rechazada.\n");
        }},
        {5, "Actualizar precio de un producto", [&]{
            Product* p = chooseProduct(store);        // Alternativa: pedir ID directo
            if (!p) { std::cout << "Producto no elegido.\n"; return; }
            float nuevo = readValue<float>("Nuevo precio: ");
            bool ok = store.updateProductPrice(p->getId(), nuevo);
            std::cout << (ok ? " Precio actualizado.\n" : " No se pudo actualizar el precio.\n");
        }},
    });
}

/**
 * @brief Punto de entrada de la aplicación de consola.
 *
 * Flujo:
 *  1. Inicializa la tienda y carga datos desde disco (juegos, películas, clientes).
 *  2. Construye el servicio de transacciones asociado a la tienda.
 *  3. Ejecuta el menú principal con submenús específicos.
 *  4. Al salir, intenta persistir en disco el estado actual del catálogo.
 *
 * Manejo de errores:
 *  - `loadFromDisk` y `saveToDisk` registran errores en STDERR sin abortar la ejecución.
 *  - Las acciones de los submenús capturan y reportan excepciones individualmente.
 *
 * @return int Código de retorno del proceso (0 si termina correctamente).
 */
int main() {
    Store store;
    store.loadFromDisk();
    TransactionService tx(store);

    run_menu_loop("MENU PRINCIPAL", {
        {1, "Administrar Productos",  [&]{ administrate_products(store, tx); }},
        {2, "Administrar Peliculas",  [&]{ administrate_movies(store); }},
        {3, "Administrar Juegos",     [&]{ administrate_games(store); }},
        {4, "Administrar Clientes",   [&]{ administrate_clients(store); }},
    }, /*exitKey*/0, /*exitLabel*/"Salir (guardar)");

    try { store.saveToDisk(); }
    catch (const std::exception& e) { std::cerr << "[save] Error: " << e.what() << "\n"; }

    std::cout << "Adiós!\n";
    return 0;
}
