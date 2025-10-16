#pragma once
#include <string>
#include "single_include/nlohmann/json.hpp"
using json = nlohmann::json;
#include "../headers/products/products.h"   // Base Product (y derivadas)
#include "../headers/client.h"              // Tu clase Client
#include "../db_utils/json_db_utils.h"
#include "../db_utils/db_enum_aliases.h"

class TransactionService {
public:
    TransactionService();

    // Alquila 'qty' unidades del producto 'product' al cliente 'client'.
    // Devuelve true si pudo realizarse y registrarse la transacción.
    bool rent(Product& product, Client& client, int qty);

    // Devuelve 'qty' unidades del producto 'product' del cliente 'client'.
    bool giveBack(Product& product, Client& client, int qty);

private:
    JSON_DB transactions_db_;   // db/transactions.json

    // Genera un id simple como string: tamaño actual + 1
    std::string next_id() const;

    // Guarda el registro en transactions.json
    bool add_transaction_record_(const std::string& action,
                                 const Product& product,
                                 const Client& client,
                                 int qty);
};
