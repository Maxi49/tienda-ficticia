#include "../../headers/transaction.h"

using json = nlohmann::json;

TransactionService::TransactionService()
    : transactions_db_(Alias::Transactions) {}

// id = cantidad de transacciones previas + 1 (como string)
std::string TransactionService::next_id() const {
    const auto& all = transactions_db_.all();
    const std::size_t n = all.is_array() ? all.size() : 0;
    return std::to_string(n + 1);
}

bool TransactionService::add_transaction_record_(const std::string& action,
                                                 const Product& product,
                                                 const Client& client,
                                                 int qty) {
    json record = {
        {"id",     next_id()},
        {"action", action},                        // "rent" o "return"
        {"qty",    qty},
        {"product", {
            {"id",   product.getId()},
            {"name", product.getName()},
            {"type", product.type()}               // "game" o "movie"
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

    // Pasamos el client_id como string para que coincida con:
    // bool Product::applyRent(const std::string& client_id, int amountReq)
    const std::string client_id_str = std::to_string(client.getId());

    // Actualizar estado en memoria
    if (!product.applyRent(client_id_str, qty)) return false;
    if (!client.addRental(product.getId(), qty)) {
        // Rollback simple si falla el cliente
        product.applyReturn(client_id_str, qty);
        return false;
    }

    // Registrar en transactions.json
    if (!add_transaction_record_("rent", product, client, qty)) {
        // Rollback si falló guardar el registro
        client.removeRental(product.getId(), qty);
        product.applyReturn(client_id_str, qty);
        return false;
    }
    return true;
}

bool TransactionService::giveBack(Product& product, Client& client, int qty) {
    if (qty <= 0) return false;

    const std::string client_id_str = std::to_string(client.getId());

    // Actualizar estado en memoria
    if (!client.removeRental(product.getId(), qty)) return false;
    if (!product.applyReturn(client_id_str, qty)) {
        // Rollback si falla el producto
        client.addRental(product.getId(), qty);
        return false;
    }

    // Registrar en transactions.json
    if (!add_transaction_record_("return", product, client, qty)) {
        // Rollback si falló guardar el registro
        product.applyRent(client_id_str, qty);
        client.addRental(product.getId(), qty);
        return false;
    }
    return true;
}
