#pragma once 
#define DB_ENUM_ALIASES_H 
#include <string>

enum class Alias : unsigned char {
  Products,
  Games,
  Store,
  Transactions,
  Clients,
  Movies,
};

/**
  DOCS:

  Simplemente un switch case para asignar
  los paths a los aliases y asi evitar tener que 
  repetir los paths en el codigo y ser propenso a errores de tipeo.

  Si se quiere cambiar un path, se cambia en este archivo.

  Si se agrega un JSON si o si hay que poner un nuevo alias y asignarle el path al JSON.

  Para usarlo en codigo:
  #include "db_enum_aliases.h"
  ejemplo de parametro

  db_utils::update_product_by_id(db_utils::Alias::Products, id, new_product);

  Atte: Maxi
*/

constexpr std::string_view to_path(Alias a) {
  switch (a) {
    case Alias::Products:     return "db/products.json";      // <- sin '/'
    case Alias::Games:        return "db/games.json";
    case Alias::Store:        return "db/store.json";
    case Alias::Clients:      return "db/clients.json";
    case Alias::Transactions: return "db/transactions.json";
    case Alias::Movies:       return "db/movies.json";
  }
  return "/db/unknown.json";
}