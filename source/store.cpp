#include "../headers/store.h"
#include "single_include/nlohmann/json.hpp"
#include <iostream>
#include <algorithm>
#include <cctype>

using nlohmann::json;


// ------------------------------------------------------------
// Helpers privados (impl.)
// ------------------------------------------------------------

/*
    Inserta o actualiza un objeto JSON en la base indicada:
      Si existe el id → update
      Si no existe → insert
    Devuelve true si se persistió en disco correctamente.
*/
bool Store::upsertJson(json obj, JSON_DB& db) {
    try {
        std::string id;
        const auto& jid = obj.at("id");
        if (jid.is_string())            id = jid.get<std::string>();
        else if (jid.is_number_integer()) id = std::to_string(jid.get<int>());
        else {
            std::cerr << "[Store::upsertJson] 'id' con tipo invalido\n";
            return false;
        }
        return db.find_by_id(id) ? db.update_by_id(std::move(obj))
                                 : db.insert(std::move(obj));
    } catch (const std::exception& e) {
        std::cerr << "[Store::upsertJson] Error: " << e.what() << "\n";
        return false;
    }
}

/*
    upsertProduct(p)
    Guarda un producto en el archivo JSON que le corresponde.

    - Convierte el objeto a JSON con to_json().
    - Según el tipo ("game" o "movie"), lo guarda en db/games.json o db/movies.json.
    - Si el tipo no es reconocido, muestra un mensaje de error.
    - Devuelve true si se guardó correctamente.

    Se usa internamente cuando se agregan, modifican o devuelven productos.
*/
bool Store::upsertProduct(const Product& p) {
    try {
        const auto j = p.to_json();          // Convierte el producto a formato JSON
        const std::string t = p.type();      // Obtiene su tipo ("game" o "movie")

        // Elige a qué base guardarlo según su tipo
        if (t == "game")  return upsertJson(j, gamesDb_);
        if (t == "movie") return upsertJson(j, moviesDb_);

        // Si llega un tipo desconocido, no corta la ejecución, solo avisa
        std::cerr << "[Store::upsertProduct] Tipo desconocido: " << t << "\n";
        return false;
    } catch (const std::exception& e) {
        // Si ocurre un error (por ejemplo, fallo al convertir o guardar), se informa
        std::cerr << "[Store::upsertProduct] Error: " << e.what() << "\n";
        return false;
    }
}

/*
    addProduct(id, make)
    Crea y guarda un nuevo producto en la tienda.

    - Usa la función "make" (una pequeña fábrica) para construir el producto.
    - Primero revisa que el ID no esté repetido en el catálogo.
    - Luego lo guarda en el archivo JSON correspondiente (upsertProduct).
    - Si todo sale bien, lo agrega al catálogo en memoria.
    - Devuelve true si el producto se creó correctamente.

    Si algo falla (por ejemplo, ID duplicado o error al guardar), devuelve false.
*/
bool Store::addProduct(int id, std::function<std::unique_ptr<Product>()> make) {
    try {
        if (catalog_.count(id)) return false;   // Evita IDs repetidos en memoria

        auto ptr = make();                      // Crea el producto con la función pasada
        if (!ptr) return false;                 // Si no se pudo crear, sale

        if (!upsertProduct(*ptr)) return false; // Lo guarda en el archivo JSON

        catalog_[id] = std::move(ptr);          // Lo agrega al catálogo en memoria
        return true;
    } catch (const std::exception& e) {
        // Si ocurre cualquier error, se informa por consola
        std::cerr << "[Store::addProduct] Error: " << e.what() << "\n";
        return false;
    }
}

/*
    loadFromDb(db, factory)
    Recorre la DB JSON y crea objetos Product concretos con la `factory`.
    Los guarda en el catálogo indexados por su ID.
    - Es genérica: se usa para Games y Movies.

    Si algún elemento del array está mal formado, se salta con seguridad.
*/
void Store::loadFromDb(
    JSON_DB& db,
    const std::function<std::unique_ptr<Product>(const json&)>& factory
) {
    const json& arr = db.all();
    if (!arr.is_array()) return;

    for (const auto& j : arr) {
        try {
            if (!j.is_object()) continue;
            if (auto p = factory(j)) {
                catalog_[p->getId()] = std::move(p);
            }
        } catch (const std::exception& e) {
            std::cerr << "[Store::loadFromDb] Item invalido: " << e.what() << "\n";
            // continúa con el resto
        }
    }
}

// util de texto: lower-case seguro para búsquedas/filtros
std::string Store::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return static_cast<char>(std::tolower(c)); });
    return s;
}

/*
    maxIdInJsonArray(arr)
    Busca el máximo "id" numérico en un array JSON. El id puede venir
    como string o como entero. Si algo no se puede parsear, se ignora.
*/
static int maxIdInJsonArray(const json& arr) {
    int mx = 0;
    if (!arr.is_array()) return 0;
    for (const auto& it : arr) {
        if (!it.is_object()) continue;
        int id = 0;
        if (it.contains("id")) {
            const auto& jv = it["id"];
            if (jv.is_string()) {
                try { id = std::stoi(jv.get<std::string>()); } catch (...) { id = 0; }
            } else if (jv.is_number_integer()) {
                id = jv.get<int>();
            }
        }
        if (id > mx) mx = id;
    }
    return mx;
}

/*
    recomputeNextIds()
    Relee los archivos JSON para determinar cuál debe ser el siguiente ID
    automático de cada categoría. Evita duplicados al volver a iniciar.
*/
void Store::recomputeNextIds() {
    try {
        nextGameId_   = maxIdInJsonArray(gamesDb_.all())   + 1;
        nextMovieId_  = maxIdInJsonArray(moviesDb_.all())  + 1;
        nextClientId_ = maxIdInJsonArray(clientsDb_.all()) + 1;
    } catch (const std::exception& e) {
        std::cerr << "[Store::recomputeNextIds] Error: " << e.what() << "\n";
        // Defaults seguros:
        nextGameId_   = std::max(nextGameId_,   1);
        nextMovieId_  = std::max(nextMovieId_,  1);
        nextClientId_ = std::max(nextClientId_, 1);
    }
}

// ------------------------------------------------------------
// Carga / Guardado
// ------------------------------------------------------------

/*
    loadFromDisk()
    1) Carga Games y Movies desde sus JSON con factorías de clase (from_json).
    2) Carga Clients (no usan Product).
    3) Recalcula los next* para que las altas automáticas sigan consecutivas.

    Si un registro está mal formado, se lo saltea sin cortar el proceso completo.
*/
void Store::loadFromDisk() {
    try {
        // 1) Games
        loadFromDb(gamesDb_, [](const json& j) -> std::unique_ptr<Product> {
            Game g = Game::from_json(j);
            return std::make_unique<Game>(g);
        });

        // 2) Movies
        loadFromDb(moviesDb_, [](const json& j) -> std::unique_ptr<Product> {
            Movie m = Movie::from_json(j);
            return std::make_unique<Movie>(m);
        });

        // 3) Clients
        const json& arr = clientsDb_.all();
        if (arr.is_array()) {
            for (const auto& j : arr) {
                try {
                    if (!j.is_object()) continue;
                    Client c = Client::from_json(j);
                    clients_[c.getId()] = std::make_unique<Client>(c);
                } catch (const std::exception& e) {
                    std::cerr << "[Store::loadFromDisk] Cliente invalido: " << e.what() << "\n";
                }
            }
        }

        // 4) Ajustar IDs automáticos
        recomputeNextIds();
    } catch (const std::exception& e) {
        std::cerr << "[Store::loadFromDisk] Error: " << e.what() << "\n";
    }
}

/*
    saveToDisk()
    Recorre el catálogo en memoria y lo vuelca a su archivo JSON correspondiente.
    Nota: los clientes se guardan al crearlos (alta) o en las transacciones.
*/
void Store::saveToDisk() {
    try {
        for (const auto& [id, ptr] : catalog_) {
            if (ptr) upsertProduct(*ptr);
        }
    } catch (const std::exception& e) {
        std::cerr << "[Store::saveToDisk] Error: " << e.what() << "\n";
    }
}


// Altas automáticas (IDs autogenerados)

/*
    addGame(...)
    Crea un juego con ID automático, lo guarda en db/games.json,
    y lo agrega al catálogo en memoria. Devuelve el ID creado o -1 si falla.
*/
int Store::addGame(const std::string& name, const std::string& genero,
                   const std::string& desc, float price, int stock,
                   const std::string& platform, const std::string& players) {
    int id = nextGameId_++;
    while (catalog_.count(id)) {      // simétrico por las dudas
        id = nextGameId_++;
    }
    return addProduct(id, [&] {
        return std::make_unique<Game>(id, name, genero, desc, price, stock, platform, players);
    }) ? id : -1;
}

/*
    addMovie(...)
    Igual que addGame pero para películas. Devuelve el ID o -1 si falla.
*/
int Store::addMovie(const std::string& name, const std::string& genero,
                    const std::string& desc, float price, int stock,
                    const std::string& director, int durationMin) {
    int id = nextMovieId_++;
    while (catalog_.count(id)) {      // evita choque con games
        id = nextMovieId_++;
    }
    return addProduct(id, [&] {
        return std::make_unique<Movie>(id, name, genero, desc, price, stock, director, durationMin);
    }) ? id : -1;
}

/*
    addClient(name)
    Crea un cliente con ID automático, lo persiste (Client::upsert)
    y lo guarda en el mapa en memoria. Devuelve el ID o -1 si falla.
*/
int Store::addClient(const std::string& name) {
    const int id = nextClientId_++;
    try {
        if (clients_.count(id)) return -1;
        auto c = std::make_unique<Client>(id, name);
        if (!Client::upsert(*c)) return -1;  // persistimos primero
        clients_[id] = std::move(c);
        return id;
    } catch (const std::exception& e) {
        std::cerr << "[Store::addClient] Error: " << e.what() << "\n";
        return -1;
    }
}

// ------------------------------------------------------------
// Consultas y búsquedas
// ------------------------------------------------------------

Product* Store::findProduct(int id) {
    auto it = catalog_.find(id);
    return (it == catalog_.end()) ? nullptr : it->second.get();
}

Client* Store::findClient(int id) {
    auto it = clients_.find(id);
    return (it == clients_.end()) ? nullptr : it->second.get();
}

/*
    Búsquedas por nombre (exacta o parcial, case-insensitive).
    Devuelven punteros “prestados” al interior de Store (no se borran externamente).
*/
std::vector<Client*> Store::findClientsByName(const std::string& name, bool partial) {
    std::vector<Client*> out;
    const std::string key = toLower(name);
    for (auto& [_, c] : clients_) {
        if (!c) continue;
        std::string nm = toLower(c->getName());
        const bool match = partial ? (nm.find(key) != std::string::npos) : (nm == key);
        if (match) out.push_back(c.get());
    }
    return out;
}

std::vector<Product*> Store::findProductsByName(const std::string& name, bool partial) {
    std::vector<Product*> out;
    const std::string key = toLower(name);
    for (auto& [_, p] : catalog_) {
        if (!p) continue;
        std::string nm = toLower(p->getName());
        const bool match = partial ? (nm.find(key) != std::string::npos) : (nm == key);
        if (match) out.push_back(p.get());
    }
    return out;
}

// Listados y filtros (salida por consola)

void Store::listProducts() const {
    if (catalog_.empty()) { std::cout << "No hay productos.\n"; return; }
    for (const auto& [id, p] : catalog_) if (p) p->showInfo();
}

void Store::listGames() const {
    bool any = false;
    for (const auto& [_, p] : catalog_) {
        if (p && p->type() == "game") { p->showInfo(); any = true; }
    }
    if (!any) std::cout << "No hay videojuegos.\n";
}

void Store::listMovies() const {
    bool any = false;
    for (const auto& [_, p] : catalog_) {
        if (p && p->type() == "movie") { p->showInfo(); any = true; }
    }
    if (!any) std::cout << "No hay peliculas.\n";
}

void Store::listClients() const {
    if (clients_.empty()) { std::cout << "No hay clientes.\n"; return; }
    for (const auto& [id, c] : clients_) if (c) c->showInfo();
}

/*
    Filtros por género (exacto). Leemos el género desde to_json()
    para no romper encapsulamiento ni exponer campos internos.
*/
void Store::listGamesByGenre(const std::string& genero) const {
    const std::string key = toLower(genero);
    bool any = false;
    for (const auto& [_, p] : catalog_) {
        if (!p || p->type() != "game") continue;
        std::string gen = toLower(p->to_json().value("genero", ""));
        if (gen == key) { p->showInfo(); any = true; }
    }
    if (!any) std::cout << "No hay videojuegos del genero '" << genero << "'.\n";
}

void Store::listMoviesByGenre(const std::string& genero) const {
    const std::string key = toLower(genero);
    bool any = false;
    for (const auto& [_, p] : catalog_) {
        if (!p || p->type() != "movie") continue;
        std::string gen = toLower(p->to_json().value("genero", ""));
        if (gen == key) { p->showInfo(); any = true; }
    }
    if (!any) std::cout << "No hay peliculas del genero '" << genero << "'.\n";
}


// Historial

/*
    
    Muestra por consola el historial de transacciones de un cliente.

    Cada registro proviene del archivo db/transactions.json e indica
    si el cliente alquiló o devolvió productos. También muestra
    el tipo de producto (game o movie) y maneja errores de formato
    sin cortar la ejecución.
*/
void Store::printClientTransactions(int clientId) const {
    JSON_DB txdb{Alias::Transactions};
    const auto& arr = txdb.all();

    if (!arr.is_array() || arr.empty()) {
        std::cout << "No hay transacciones registradas.\n";
        return;
    }

    bool found = false;

    for (const auto& t : arr) {
        try {
            // ---- Validar que exista "client" y "id" ----
            if (!t.contains("client") || !t["client"].is_object()) {
                std::cerr << "[Store::printClientTransactions] Registro invalido: falta objeto 'client'\n";
                continue;
            }
            const auto& jc = t["client"];
            if (!jc.contains("id")) {
                std::cerr << "[Store::printClientTransactions] Registro invalido: falta 'client.id'\n";
                continue;
            }

            // ---- Parsear client.id (string o number) ----
            int cid = -1;
            const auto& jcid = jc["id"];
            if (jcid.is_string()) {
                try { cid = std::stoi(jcid.get<std::string>()); }
                catch (...) { cid = -1; }
            } else if (jcid.is_number_integer()) {
                cid = jcid.get<int>();
            } else if (jcid.is_number()) {
                cid = static_cast<int>(jcid.get<double>());
            }

            if (cid != clientId) continue; // no es el cliente buscado

            found = true;

            // ---- action ----
            const std::string action = t.value("action", "?");
            std::string actionText =
                (action == "rent")   ? "Alquilo" :
                (action == "return") ? "Devolvio" : "(acción desconocida)";

            // ---- qty ----
            int qty = 0;
            try { qty = t.value("qty", 0); } catch (...) {}

            // ---- product ----
            int pid = -1;
            std::string pname = "(sin nombre)";
            std::string ptype = "(sin tipo)";

            if (t.contains("product") && t["product"].is_object()) {
                const auto& jp = t["product"];
                if (jp.contains("id")) {
                    const auto& jpid = jp["id"];
                    if (jpid.is_string()) {
                        try { pid = std::stoi(jpid.get<std::string>()); } catch (...) { pid = -1; }
                    } else if (jpid.is_number_integer()) {
                        pid = jpid.get<int>();
                    } else if (jpid.is_number()) {
                        pid = static_cast<int>(jpid.get<double>());
                    }
                }
                pname = jp.value("name", pname);
                ptype = jp.value("type", ptype);
            } else {
                std::cerr << "[Store::printClientTransactions] Registro invalido: falta objeto 'product'\n";
            }

            std::cout << "- " << actionText << " " << qty
                      << " x [" << pid << "] "
                      << pname << " (" << ptype << ")\n";

        } catch (const std::exception& e) {
            std::cerr << "[Store::printClientTransactions] Registro invalido: " << e.what() << "\n";
        }
    }

    if (!found) {
        std::cout << "El cliente no tiene transacciones registradas.\n";
    }
}


/*
    upsert(p)
    API pública usada por TransactionService para persistir el snapshot de un producto.
    Envuelve upsertProduct con try/catch para no romper el flujo de la transacción.
*/
bool Store::upsert(const Product& p) {
    try {
        return upsertProduct(p);
    } catch (const std::exception& e) {
        std::cerr << "[Store::upsert] Error: " << e.what() << "\n";
        return false;
    }
}

bool Store::updateProductPrice(int productId, float newPrice) {
    Product* p = findProduct(productId);
    if (!p) {
        std::cerr << "[Store::updateProductPrice] Producto no encontrado.\n";
        return false;
    }
    if (newPrice < 0.0f || !std::isfinite(newPrice)) {
        std::cerr << "[Store::updateProductPrice] Precio inválido.\n";
        return false;
    }

    // 1. Tomamos el JSON actual del producto
    nlohmann::json j = p->to_json();
    j["price"] = newPrice;  // mutación

    // 2. Persistimos usando la lógica que ya existe
    const std::string t = p->type();
    JSON_DB* db = nullptr;
    if      (t == "game")  db = &gamesDb_;
    else if (t == "movie") db = &moviesDb_;
    else {
        std::cerr << "[Store::updateProductPrice] Tipo desconocido.\n";
        return false;
    }

    if (!upsertJson(std::move(j), *db)) {
        std::cerr << "[Store::updateProductPrice] Falló upsertJson().\n";
        return false;
    }

    // 3. Recargamos el producto en memoria desde la base actualizada
    auto maybeJ = db->find_by_id(std::to_string(productId));
    if (!maybeJ) return false;

    if (t == "game") {
        Game g = Game::from_json(*maybeJ);
        catalog_[productId] = std::make_unique<Game>(g);
    } else if (t == "movie") {
        Movie m = Movie::from_json(*maybeJ);
        catalog_[productId] = std::make_unique<Movie>(m);
    }

    return true;
}


MovieInput Store::readMovieInput() {
    return  {
        readValue<std::string>("name: "),
        readValue<std::string>("genero: "),
        readValue<std::string>("description: "),
        readValue<std::string>("director: "),
        readValue<float>("price: "),
        readValue<int>("totalStock: "),
        readValue<int>("durationMin: ")
    };
};

GameInput Store::readGameInput() {
    return  {
        readValue<std::string>("name: "),
        readValue<std::string>("genero: "),
        readValue<std::string>("description: "),
        readValue<std::string>("platform: "),
        readValue<std::string>("players: "),
        readValue<float>("price: "),
        readValue<int>("stock: ")
    };
};

ClientInput Store::readClientInput() {
    return {readValue<std::string>("name: ") };
}

bool Store::deleteProduct(int productId) {
    try {
        const std::string sid = std::to_string(productId);

        if (Product* p = findProduct(productId)) {
            const std::string t = p->type();
            JSON_DB* db = nullptr;
            if      (t == "game")  db = &gamesDb_;
            else if (t == "movie") db = &moviesDb_;
            else {
                std::cerr << "[Store::deleteProduct] Tipo desconocido para id=" << productId << ".\n";
                return false;
            }

            if (!db->delete_by_id(sid)) {
                std::cerr << "[Store::deleteProduct] No se pudo borrar id=" << productId
                          << " de la DB (" << t << ").\n";
                return false;
            }

            // Borramos del catálogo en memoria.
            catalog_.erase(productId);
            return true;
        }

        bool ok = false;
        if (gamesDb_.delete_by_id(sid))  ok = true;
        if (moviesDb_.delete_by_id(sid)) ok = true;

        if (!ok) {
            std::cerr << "[Store::deleteProduct] No existe el id " << productId
                      << " en games.json ni movies.json.\n";
            return false;
        }

        catalog_.erase(productId);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Store::deleteProduct] Error: " << e.what() << "\n";
        return false;
    }
}

bool Store::hasTxForClient_(int clientId) {
    try {
        JSON_DB txdb{Alias::Transactions};
        const auto& arr = txdb.all();
        if (!arr.is_array()) return false;

        for (const auto& t : arr) {
            try {
                const auto& jcid = t.at("client").at("id");
                int cid = -1;
                if (jcid.is_string())      cid = std::stoi(jcid.get<std::string>());
                else if (jcid.is_number()) cid = jcid.get<int>();
                if (cid == clientId) return true;
            } catch (...) {
                // registro mal formado -> lo ignoramos
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Store::hasTxForClient_] Error: " << e.what() << "\n";
    }
    return false;
}

// --- borrar cliente: DB -> memoria (con política de seguridad) ---
bool Store::deleteClient(int clientId, bool force) {
    try {
        // 0) política de integridad: no borrar si tiene historial (salvo force)
        if (!force && hasTxForClient_(clientId)) {
            std::cerr << "[Store::deleteClient] El cliente id=" << clientId
                      << " tiene transacciones. Use force=true si desea borrarlo igualmente.\n";
            return false;
        }

        const std::string sid = std::to_string(clientId);

        // 1) borrar en disco
        if (!clientsDb_.delete_by_id(sid)) {
            std::cerr << "[Store::deleteClient] No existe id=" << clientId << " en clients.json.\n";
            return false;
        }

        // 2) borrar en memoria (si estaba)
        clients_.erase(clientId);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "[Store::deleteClient] Error: " << e.what() << "\n";
        return false;
    }
}

void Store::listClientsBrief() const {
    if (clients_.empty()) {
        std::cout << "No hay clientes.\n";
        return;
    }
    std::cout << "Clientes disponibles:\n";
    for (const auto& [id, c] : clients_) {
        if (!c) continue;
        std::cout << "  [" << id << "] " << c->getName() << "\n";
    }
}



