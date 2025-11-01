#pragma once
#include <string>
#include "single_include/nlohmann/json.hpp"
using json = nlohmann::json;
#include "products/products.h"
#include "client.h"
#include "store.h"
#include "../db_utils/json_db_utils.h"
#include "../db_utils/db_enum_aliases.h"

class TransactionService {
public:
    explicit TransactionService(Store& store);

    bool rent(Product& product, Client& client, int qty);
    bool giveBack(Product& product, Client& client, int qty);

private:
    JSON_DB transactions_db_;   // db/transactions.json
    Store&  store_;             // referencia (no owning)

    std::string next_id() const;
    bool add_transaction_record_(const std::string& action,
                                 const Product& product,
                                 const Client& client,
                                 int qty);
};
