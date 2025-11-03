#include "../headers/transaction.h"
using json = nlohmann::json;
// Constructor
TransactionService::TransactionService(Store& s)
    : transactions_db_(Alias::Transactions),
      store_(s) {}
// Genera el próximo ID de transacción
std::string TransactionService::next_id() const {
    const auto& all = transactions_db_.all();
    const std::size_t n = all.is_array() ? all.size() : 0;
    return std::to_string(n + 1);
}
// Agrega un registro de transacción (alquiler o devolución)
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
            {"id", std::to_string(product.getId())},
            {"name", product.getName()},
            {"type", product.type()}
        }},
        {"client", {
            {"id", std::to_string(client.getId())},
            {"name", client.getName()}
        }}
    };
    return transactions_db_.insert(std::move(record));
}
// Realiza el alquiler de un producto a un cliente
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

    // Snapshots (no romper log si fallan)
    Client::upsert(client);    // db/clients.json
    store_.upsert(product);    // db/games.json / db/movies.json
    return true;
}

// Realiza la devolución de un producto por parte de un cliente
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

    Client::upsert(client);
    store_.upsert(product);
    return true;
}
