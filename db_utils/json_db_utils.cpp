#include "json_db_utils.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <system_error>

#include "db_enum_aliases.h"
#include "single_include/nlohmann/json.hpp"

using json = nlohmann::json;

JSON_DB::JSON_DB(Alias path_alias)
    : path_alias_(path_alias),
      file_path_(resolve_path_(path_alias)),
      data_(json::array()) {
  load_();
}

std::filesystem::path JSON_DB::resolve_path_(Alias path_alias) {
  std::string raw_path = to_path(path_alias);
  if (!raw_path.empty() && (raw_path.front() == '/' || raw_path.front() == '\\')) {
    raw_path.erase(raw_path.begin());
  }

  std::filesystem::path resolved(raw_path);
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

  std::ofstream output(file_path_, std::ios::binary | std::ios::trunc);
  if (!output.is_open()) {
    std::cerr << "No se pudo abrir el archivo " << file_path_.string() << " para escritura."
              << std::endl;
    return false;
  }

  output << std::setw(2) << data_;
  if (!output.good()) {
    std::cerr << "Fallo al escribir los datos en " << file_path_.string() << std::endl;
    return false;
  }

  return true;
}

std::optional<std::size_t> JSON_DB::index_of_id_(const std::string& id) const {
  for (std::size_t i = 0; i < data_.size(); ++i) {
    const auto& item = data_[i];
    const auto it = item.find("id");
    if (it != item.end() && it->is_string() && *it == id) {
      return i;
    }
  }
  return std::nullopt;
}

std::optional<json> JSON_DB::find_by_id(const std::string& id) const {
  if (auto idx = index_of_id_(id)) {
    return std::optional<json>data_[*idx];
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
