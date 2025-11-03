#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "../db_utils/json_db_utils.h"
#include "../db_utils/db_enum_aliases.h"
#include "products/game.h"
#include "products/movie.h"
#include "client.h"
#include "../utils/templates.h"

struct MovieInput {
    std::string name, genre, description, director;
    float price;
    int stock, duration;
};


struct GameInput {
    std::string name, genre, description, platform, players;
    float price;
    int stock;
};

struct ClientInput {
    std::string name;
};


/*
    Clase Store
    Representa la "tienda" principal del sistema.

    Administra todo el catálogo de productos (juegos y películas).
    Gestiona los clientes y sus alquileres.
    Se encarga de guardar y cargar los datos en archivos JSON dentro de /db.
    Es utilizada por TransactionService para mantener los datos sincronizados cuando hay un alquiler o una devolución.
    Manejo de punteros: cada producto y cliente se almacena usando un std::unique_ptr.
*/
class Store {
    // Bases de datos JSON (archivos de respaldo del disco)
    JSON_DB gamesDb_{Alias::Games};
    JSON_DB moviesDb_{Alias::Movies};
    JSON_DB clientsDb_{Alias::Clients};

    // Datos en memoria (almacenados como punteros únicos)
    // Cada producto o cliente pertenece solo a la tienda.
    std::unordered_map<int, std::unique_ptr<Product>> catalog_;
    std::unordered_map<int, std::unique_ptr<Client>>  clients_;

    // Contadores para IDs automáticos (se recalculan al cargar desde json)
    int nextGameId_   = 1;
    int nextMovieId_  = 1;
    int nextClientId_ = 1;

    // Metodos internos
    static bool upsertJson(nlohmann::json obj, JSON_DB& db); // inserta o actualiza un objeto JSON
    bool upsertProduct(const Product& p);                    // guarda o actualiza el producto según su tipo
    bool addProduct(int id, std::function<std::unique_ptr<Product>()> make);
    void loadFromDb(JSON_DB& db,
                    const std::function<std::unique_ptr<Product>(const nlohmann::json&)>& factory);
    void recomputeNextIds();                                 // ajusta los contadores de ID existentes
    static std::string toLower(std::string s);               // convierte texto a minúsculas (para búsquedas)

public:
    //GUARDADO
    bool upsert(const Product& p);
    /*
        Si el producto no existe, lo inserta (INSERT).
        Si ya existe, actualiza sus datos (UPDATE).
        Determina automáticamente el archivo destino según el tipo (game o movie).
    */
    //CARGA Y GUARDADO GENERAL
    void loadFromDisk(); // carga productos y clientes desde los archivos JSON
    void saveToDisk();   // guarda el estado actual del catálogo y los clientes

    //ALTAS AUTOMÁTICAS (quiere decir que no incluyo el id en el constructor porque lo crea el sistema)
    int addGame(const std::string& name, const std::string& genero,
                    const std::string& desc, float price, int stock,
                    const std::string& platform, const std::string& players);

    int addMovie(const std::string& name, const std::string& genero,
                     const std::string& desc, float price, int stock,
                     const std::string& director, int durationMin);

    int addClient(const std::string& name);

    // ----- CONSULTAS -----
    Product* findProduct(int id); // busca un producto por su ID
    Client*  findClient(int id);  // busca un cliente por su ID

    // Búsquedas por nombre
    std::vector<Client*>  findClientsByName(const std::string& name, bool partial = true);
    std::vector<Product*> findProductsByName(const std::string& name, bool partial = true);

    // ----- LISTADOS -----
    void listProducts() const; // lista todo el catálogo
    void listGames() const;    // solo videojuegos
    void listMovies() const;   // solo películas
    void listClients() const;  // todos los clientes

    // Filtros por género
    void listGamesByGenre(const std::string& genero) const;
    void listMoviesByGenre(const std::string& genero) const;

    // Muestra las transacciones registradas de un cliente
    void printClientTransactions(int clientId) const;

    MovieInput readMovieInput();

    GameInput readGameInput();

    ClientInput readClientInput();


};
