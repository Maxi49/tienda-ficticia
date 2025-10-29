// headers/transaction.h
#pragma once
#include <string>
#include "single_include/nlohmann/json.hpp"
using json = nlohmann::json;
#include "../headers/products/products.h"
#include "../headers/client.h"
#include "../db_utils/json_db_utils.h"
#include "../db_utils/db_enum_aliases.h"

class TransactionService {
public:
    TransactionService();

    // Transacciones
    bool rent(Product& product, Client& client, int qty);
    bool giveBack(Product& product, Client& client, int qty);

    // >>> NUEVO: registrar/actualizar cliente sin transacción (upsert)
    bool registerClient(const Client& client);

private:
    JSON_DB transactions_db_;   // db/transactions.json
    JSON_DB clients_db_;        // db/clients.json

    std::string next_id() const;
    bool add_transaction_record_(const std::string& action,
                                 const Product& product,
                                 const Client& client,
                                 int qty);

    // Guarda/actualiza al cliente (usada por rent/return y por registerClient)
    bool persist_client_snapshot_(const Client& client);
};
