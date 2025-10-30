#pragma once
#include "../db_utils/json_db_utils.h"
#include "../db_utils/db_enum_aliases.h"
#include "products/game.h"
#include "products/movie.h"

class Store {
    JSON_DB games_{Alias::Games};     // db/games.json
    JSON_DB movies_{Alias::Movies};   // db/movies.json

    static bool upsert_(nlohmann::json obj, JSON_DB& db) {
        const std::string id = obj["id"].get<std::string>();
        return db.find_by_id(id) ? db.update_by_id(std::move(obj))
                                 : db.insert(std::move(obj));
    }

public:
    bool upsert(const Game& g)  { return upsert_(g.to_json(),  games_); }
    bool upsert(const Movie& m) { return upsert_(m.to_json(), movies_); }

    // Útil para TransactionService con Product&
    bool upsert(const Product& p);
};
