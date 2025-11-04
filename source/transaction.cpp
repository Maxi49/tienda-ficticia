#include "../headers/transaction.h"
using json = nlohmann::json;

/*
TransactionService
Recibe un Store por referencia
Abre/usa la DB de transacciones (db/transactions.json).
*/
TransactionService::TransactionService(Store& s)
    : transactions_db_(Alias::Transactions),
      store_(s) {}

/*
next_id()
Calcula el próximo ID de transacción como (tamaño_del_array + 1).
Si la DB aún no existe o está vacía, arranca en "1".
*/

std::string TransactionService::next_id() const {
    const auto& all = transactions_db_.all();
    const std::size_t n = all.is_array() ? all.size() : 0;
    return std::to_string(n + 1);
}

/*
add_transaction_record_
Registra una transacción mínima (alquiler o devolución (una base de como es el json que se guarda).
Guarda: id, acción ("rent"/"return"), cantidad,
    y hace actualizaciones básicas en producto y cliente.
Devuelve true si la inserción en JSON fue OK.
*/

bool TransactionService::add_transaction_record_(
    const std::string& action,
    const Product& product,
    const Client& client,
    int qty) 
{
    try {
        json record = {
            {"id",     next_id()},
            {"action", action},
            {"qty",    qty},
            {"product", {
                {"id",   std::to_string(product.getId())},
                {"name", product.getName()},
                {"type", product.type()}
            }},
            {"client", {
                {"id",   std::to_string(client.getId())},
                {"name", client.getName()}
            }}
        };

        if (!transactions_db_.insert(std::move(record))) { //se guarda en el disco con la funcion insert de db utils
            std::cerr << "[TransactionService::add_transaction_record_] insert() devolvió false\n";
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[TransactionService::add_transaction_record_] Excepción: " << e.what() << "\n";
        return false;
    }
}

/*
// rent(product, client, qty)
    Valida cantidad y stock.
    Aplicar al producto (baja stock + marca alquiler).
    Aplicar al cliente (agrega línea de alquiler).
    Registrar transacción. Si falla, se revierte 2 y 3.
    Persistir actualizaciones (Cliente y Producto). Si falla, no se revierte la transaccion, tira un warning de que no se pudo guardar
*/
bool TransactionService::rent(Product& product, Client& client, int qty) {
    
    if (qty <= 0) {
        std::cerr << "[TransactionService::rent] qty <= 0\n";
        return false;
    }
    if (!product.canRent(qty)) {
        std::cerr << "[TransactionService::rent] sin stock suficiente\n";
        return false;
    }

    const std::string client_id_str = std::to_string(client.getId());

    // aplica al producto (maneja el stock y todo eso)
    if (!product.applyRent(client_id_str, qty)) {
        std::cerr << "[TransactionService::rent] applyRent() rechazó la operación\n";
        return false;
    }

    // aplica al cliente (se pone en active rentals) (si falla, revertimos producto)
    if (!client.addRental(product.getId(), qty)) {
        std::cerr << "[TransactionService::rent] addRental() rechazó la operación; se revierte producto\n";
        product.applyReturn(client_id_str, qty);
        return false;
    }

    // registrar transacción (si falla, revertimos ambos)
    if (!add_transaction_record_("rent", product, client, qty)) {
        std::cerr << "[TransactionService::rent] no se pudo registrar transacción; se revierte todo\n";
        client.removeRental(product.getId(), qty);
        product.applyReturn(client_id_str, qty);
        return false;
    }

    // actualizaciones en disco (no se rompe  si falla la transacción)
    if (!Client::upsert(client)) {
        std::cerr << "[TransactionService::rent] ADVERTENCIA: snapshot de cliente no guardado\n";
    }
    if (!store_.upsert(product)) {
        std::cerr << "[TransactionService::rent] ADVERTENCIA: snapshot de producto no guardado\n";
    }

    return true;
}

/*
giveBack(product, client, qty)
    Sigue la misma logica que rent(), pero al reves.
*/

bool TransactionService::giveBack(Product& product, Client& client, int qty) {
    if (qty <= 0) {
        std::cerr << "[TransactionService::giveBack] qty <= 0\n";
        return false;
    }

    const std::string client_id_str = std::to_string(client.getId());

    // 2) primero “saca” del cliente
    if (!client.removeRental(product.getId(), qty)) {
        std::cerr << "[TransactionService::giveBack] removeRental() rechazó la operación\n";
        return false;
    }

    // 3) luego devuelve stock al producto (si falla, reponemos al cliente)
    if (!product.applyReturn(client_id_str, qty)) {
        std::cerr << "[TransactionService::giveBack] applyReturn() rechazó; se repone en cliente\n";
        client.addRental(product.getId(), qty);
        return false;
    }

    // 4) registrar transacción (si falla, revertimos ambos)
    if (!add_transaction_record_("return", product, client, qty)) {
        std::cerr << "[TransactionService::giveBack] no se pudo registrar transacción; se revierte todo\n";
        product.applyRent(client_id_str, qty);
        client.addRental(product.getId(), qty);
        return false;
    }

    // 5) actualizacion en la db (
    if (!Client::upsert(client)) {
        std::cerr << "[TransactionService::giveBack] Advertencia: snapshot de cliente no guardado\n";
    }
    if (!store_.upsert(product)) {
        std::cerr << "[TransactionService::giveBack] Advertencia: snapshot de producto no guardado\n";
    }

    return true;
}
