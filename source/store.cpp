#include "../headers/store.h"
#include <iostream>

Store::Store()
	: clients_(), products_(), tx_(), next_client_id_(1), next_product_id_(1) {}

int Store::createClient(const std::string& name) {
	int id = next_client_id_++;
	clients_.emplace_back(id, name);
	return id;
}

int Store::createGame(const std::string& name,
					  const std::string& genero,
					  const std::string& description,
					  float price,
					  int totalStock,
					  const std::string& platform,
					  const std::string& players) {
	int id = next_product_id_++;
	products_.push_back(std::make_unique<Game>(id, name, genero, description, price, totalStock, platform, players));
	return id;
}

int Store::createMovie(const std::string& name,
					   const std::string& genero,
					   const std::string& description,
					   float price,
					   int totalStock,
					   const std::string& director,
					   int durationMin) {
	int id = next_product_id_++;
	products_.push_back(std::make_unique<Movie>(id, name, genero, description, price, totalStock, director, durationMin));
	return id;
}

void Store::listClients() const {
	std::cout << "--- Clientes (" << clients_.size() << ") ---" << std::endl;
	for (const auto& c : clients_) {
		c.showInfo();
	}
}

void Store::listProducts() const {
	std::cout << "--- Productos (" << products_.size() << ") ---" << std::endl;
	for (const auto& p : products_) {
		p->showInfo();
	}
}

Client* Store::findClient_(int id) {
	for (auto& c : clients_) {
		if (c.getId() == id) return &c;
	}
	return nullptr;
}

Product* Store::findProduct_(int id) {
	for (auto& p : products_) {
		if (p->getId() == id) return p.get();
	}
	return nullptr;
}

bool Store::rentProduct(int productId, int clientId, int qty) {
	// Mostrar al usuario los productos que están disponibles antes de procesar el alquiler
	listAvailableProducts();

	Client* c = findClient_(clientId);
	Product* p = findProduct_(productId);
	if (!c || !p) return false;
	return tx_.rent(*p, *c, qty);
}

bool Store::returnProduct(int productId, int clientId, int qty) {
	Client* c = findClient_(clientId);
	Product* p = findProduct_(productId);
	if (!c || !p) return false;
	return tx_.giveBack(*p, *c, qty);
}

void Store::showClientInfo(int clientId) const {
	for (const auto& c : clients_) {
		if (c.getId() == clientId) {
			c.showInfo();
			return;
		}
	}
	std::cout << "Cliente con id " << clientId << " no encontrado." << std::endl;
}

void Store::showProductInfo(int productId) const {
	for (const auto& p : products_) {
		if (p->getId() == productId) {
			p->showInfo();
			return;
		}
	}
	std::cout << "Producto con id " << productId << " no encontrado." << std::endl;
}

void Store::listAvailableProducts() const {
	std::cout << "--- Productos disponibles ---" << std::endl;
	bool any = false;
	for (const auto& p : products_) {
		if (p->getAvailableStock() > 0) {
			std::cout << "ID: " << p->getId()
					  << " - " << p->getName()
					  << " (Disponibles: " << p->getAvailableStock() << ")"
					  << " - Precio: " << p->getPrice()
					  << std::endl;
			any = true;
		}
	}
	if (!any) {
		std::cout << "No hay productos disponibles para alquilar." << std::endl;
	}
}

