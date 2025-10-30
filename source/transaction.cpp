#include "../headers/transaction.h"
using json = nlohmann::json;

TransactionService::TransactionService()
    : transactions_db_(Alias::Transactions),
      store_() {}

std::string TransactionService::next_id() const {
    const auto& all = transactions_db_.all();
    const std::size_t n = all.is_array() ? all.size() : 0;
    return std::to_string(n + 1);
}

bool TransactionService::add_transaction_record_(
    const std::string& action,
    const Product& product,
    const Client& client,
    int qty) {

    json record = {
        {"id",     next_id()},
        {"action", action},
        {"qty",    qty},
        {"product", {
            {"id",   product.getId()},
            {"name", product.getName()},
            {"type", product.type()}
        }},
        {"client", {
            {"id",   client.getId()},
            {"name", client.getName()}
        }}
    };
    return transactions_db_.insert(std::move(record));
}

bool TransactionService::rent(Product& product, Client& client, int qty) {
    if (qty <= 0) return false;
    if (!product.canRent(qty)) return false;

    const std::string client_id_str = std::to_string(client.getId());

    if (!product.applyRent(client_id_str, qty)) return false;
    if (!client.addRental(product.getId(), qty)) {
        product.applyReturn(client_id_str, qty);
        return false;
    }

    if (!add_transaction_record_("rent", product, client, qty)) {
        client.removeRental(product.getId(), qty);
        product.applyReturn(client_id_str, qty);
        return false;
    }

    // ---- Persistencias de snapshots (no rompen el log si fallan) ----
    Client::upsert(client);     // db/clients.json
    store_.upsert(product);     // db/games.json / db/movies.json

    return true;
}

bool TransactionService::giveBack(Product& product, Client& client, int qty) {
    if (qty <= 0) return false;

    const std::string client_id_str = std::to_string(client.getId());

    if (!client.removeRental(product.getId(), qty)) return false;
    if (!product.applyReturn(client_id_str, qty)) {
        client.addRental(product.getId(), qty);
        return false;
    }

    if (!add_transaction_record_("return", product, client, qty)) {
        product.applyRent(client_id_str, qty);
        client.addRental(product.getId(), qty);
        return false;
    }

    // ---- Persistencias de snapshots ----
    Client::upsert(client);
    store_.upsert(product);

    return true;
}
