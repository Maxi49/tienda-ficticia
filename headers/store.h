
#pragma once

#include <vector>
#include <memory>
#include <string>

#include "client.h"
#include "transaction.h"
#include "products/game.h"
#include "products/movie.h"

class Store {
public:
	Store();

	// Creación automática de clientes y productos (ids internos)
	int createClient(const std::string& name);

	int createGame(const std::string& name,
				   const std::string& genero,
				   const std::string& description,
				   float price, 
				   int totalStock,
				   const std::string& platform,
				   const std::string& players);

	int createMovie(const std::string& name,
					const std::string& genero,
					const std::string& description,
					float price,
					int totalStock,
					const std::string& director,
					int durationMin);

	// Listados
	void listClients() const;
	void listProducts() const;

	// Listar sólo los productos con stock disponible (mostrando ids)
	void listAvailableProducts() const;

	// Operaciones de transacción (usa TransactionService internamente)
	bool rentProduct(int productId, int clientId, int qty);
	bool returnProduct(int productId, int clientId, int qty);

	// Mostrar info individual
	void showClientInfo(int clientId) const;
	void showProductInfo(int productId) const;

private:
	std::vector<Client> clients_;
	std::vector<std::unique_ptr<Product>> products_;
	TransactionService tx_;

	int next_client_id_;
	int next_product_id_;

	Client* findClient_(int id);
	Product* findProduct_(int id);
};
