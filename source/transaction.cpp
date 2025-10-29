// source/transaction.cpp
#include "../headers/transaction.h"
using json = nlohmann::json;

TransactionService::TransactionService()
    : transactions_db_(Alias::Transactions),
      clients_db_(Alias::Clients) {}

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

// >>> NUEVO: alta/actualización de cliente sin transacción
bool TransactionService::registerClient(const Client& client) {
    return persist_client_snapshot_(client);
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

    // snapshot de cliente
    persist_client_snapshot_(client);
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

    // snapshot de cliente
    persist_client_snapshot_(client);
    return true;
}

// Upsert del cliente en db/clients.json usando tu JSON_DB
bool TransactionService::persist_client_snapshot_(const Client& client) {
    json obj = client.to_json(); // {"id":"...","name":"...","rentals":[...]}

    const std::string idStr = obj["id"].get<std::string>();
    if (clients_db_.find_by_id(idStr)) {
        return clients_db_.update_by_id(std::move(obj));
    } else {
        return clients_db_.insert(std::move(obj));
    }
}
