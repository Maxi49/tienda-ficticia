#include "../headers/store.h"

bool Store::upsert(const Product& p) {
    if (p.type() == "game") {
        // cast seguro si querés campos extra (no obligatorio para guardar)
        return upsert_(p.to_json(), games_);
    }
    if (p.type() == "movie") {
        return upsert_(p.to_json(), movies_);
    }
    return true; // otros tipos no persistidos
}
