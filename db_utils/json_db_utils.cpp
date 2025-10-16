#include "json_db_utils.h"
#include <optional>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <system_error>

#include "db_enum_aliases.h"
#include "single_include/nlohmann/json.hpp"

/*
  DOCS: 

  importar db_enum_aliases.h (ver archivo)

  dentro de las clases agregar:

  JSON_DB nombre_de_clase_db{Alias::Products};

  luego se puede usar Clase::db_.insert(obj), Clase::db_.find_by_id(id), etc.

  ejemplos:

  JSON_DB products_db{Alias::Products};

  insert: 

  json new_product = {
      {"id", "123"},
      {"name", "Producto de prueba"},
      {"price", 19.99}
  };

  if (products_db.insert(new_product)) {
      std::cout << "Producto insertado exitosamente." << std::endl;
  } else {
      std::cout << "Fallo al insertar el producto (posiblemente id duplicado)." << std::endl;
  }

  buscar por id:

  auto producto = products_db.find_by_id("id_producto");

  if (!producto) {
      std::cerr << "Producto no encontrado." << std::endl;
  }
  
  std::cout << "Producto encontrado: " << producto->dump() << std::endl;    


  actualizar por id:

  json updated_product = {
      {"id", "123"},
      {"name", "Producto actualizado"},
      {"price", 24.99}
  };

  if (products_db.update_by_id(updated_product)) {
      std::cout << "Producto actualizado exitosamente." << std::endl;
  } else {
      std::cout << "Fallo al actualizar el producto (posiblemente no existe)." << std::endl;
  }

  y para eliminar:

  if (products_db.delete_by_id("123")) {
      std::cout << "Producto eliminado exitosamente." << std::endl;
  } else {
      std::cout << "Fallo al eliminar el producto (posiblemente no existe)." << std::endl;
  }

*/

using json = nlohmann::json;

JSON_DB::JSON_DB(Alias path_alias)
    : path_alias_(path_alias),
      file_path_(resolve_path_(path_alias)),
      data_(json::array()) {
  load_();
}

std::filesystem::path JSON_DB::resolve_path_(Alias path_alias) {
  std::filesystem::path resolved{std::string(to_path(path_alias))};

  if (resolved.is_absolute()) {
    return resolved.lexically_normal();
  }

  std::error_code ec;
  const auto base = std::filesystem::current_path(ec);
  if (ec) {
    std::cerr << "No se pudo obtener el directorio actual: " << ec.message() << std::endl;
    return resolved.lexically_normal();
  }
  return (base / resolved).lexically_normal();
}
void JSON_DB::load_() {
  std::ifstream input(file_path_);
  if (!input.is_open()) {
    data_ = json::array();
    return;
  }

  try {
    json parsed;
    input >> parsed;
    if (!parsed.is_array()) {
      std::cerr << "El archivo JSON " << file_path_.string()
                << " no contiene un array como raiz. Se reiniciara en blanco." << std::endl;
      data_ = json::array();
      return;
    }
    data_ = std::move(parsed);
  } catch (const json::parse_error& e) {
    std::cerr << "Error al parsear " << file_path_.string() << ": " << e.what() << std::endl;
    data_ = json::array();
  }
}

bool JSON_DB::save_() {
  const auto parent = file_path_.parent_path();
  if (!parent.empty()) {
    std::error_code ec;
    std::filesystem::create_directories(parent, ec);
    if (ec) {
      std::cerr << "No se pudo crear el directorio " << parent.string()
                << ": " << ec.message() << std::endl;
      return false;
    }
  }

  auto temp_path = file_path_;
  temp_path += ".tmp";

  {
    std::ofstream tmp(temp_path, std::ios::binary | std::ios::trunc);
    if (!tmp.is_open()) {
      std::cerr << "No se pudo abrir " << temp_path.string() << " para escritura.\n";
      return false;
    }
    tmp << std::setw(2) << data_;
    if (!tmp.good()) {
      std::cerr << "Fallo al escribir datos en " << temp_path.string() << "\n";
      return false;
    }
    tmp.flush();
    // (Opcional) fsync vía descriptor si querés máxima durabilidad.
  }

  std::error_code ec;
  std::filesystem::rename(temp_path, file_path_, ec);
  if (ec) {
    // En Windows, si el destino existe puede fallar. Probá remove + rename.
    std::filesystem::remove(file_path_, ec); // ignorá error
    ec.clear();
    std::filesystem::rename(temp_path, file_path_, ec);
    if (ec) {
      std::cerr << "No se pudo renombrar " << temp_path.string()
                << " a " << file_path_.string() << ": " << ec.message() << "\n";
      return false;
    }
  }

  return true;
}

std::optional<std::size_t> JSON_DB::index_of_id_(const std::string& id) const {
  for (std::size_t i = 0; i < data_.size(); ++i) {
    const auto& item = data_[i];
    auto it = item.find("id");
    if (it == item.end()) continue;

    if (it->is_string() && *it == id) return i;
    if (it->is_number() && std::to_string(it->get<long long>()) == id) return i;
  }
  return std::nullopt;
}

const json& JSON_DB::all() const { return data_; }

std::optional<json> JSON_DB::find_by_id(const std::string& id) const {
  if (auto idx = index_of_id_(id)) {
    return std::optional<json>(data_[*idx]);
  }

  return std::nullopt;
}

bool JSON_DB::insert(json obj) {
  if (!obj.contains("id")) return false;
  const json& id_field = obj["id"];
  if (!id_field.is_string()) return false;

  const std::string id = id_field.get<std::string>();
  if (index_of_id_(id)) {
    return false;
  }

  data_.push_back(std::move(obj));
  return save_();
}

bool JSON_DB::update_by_id(json obj) {
  if (!obj.contains("id")) return false;

  const json& id_field = obj["id"];
  if (!id_field.is_string()) return false;

  const std::string id = id_field.get<std::string>();

  if (auto idx = index_of_id_(id)) {
    data_[*idx] = std::move(obj);
    return save_();
  }

  std::cerr << "No se encontro el item con id: " << id << " para actualizar." << std::endl;
  return false;
}

bool JSON_DB::delete_by_id(const std::string& id) {
  if (auto idx = index_of_id_(id)) {
    data_.erase(data_.begin() + static_cast<std::ptrdiff_t>(*idx));
    return save_();
  }
  return false;
}

std::vector<json> JSON_DB::search_by_class_field(const std::string& field, const json& value) const {
  std::vector<json> matches;
  for (const auto& item : data_) {
    if (!item.is_object()) continue;
    const auto it = item.find(field);
    if (it != item.end() && *it == value) {
      matches.push_back(item);
    }
  }
  return matches;
}
