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

using json = nlohmann::json;

/*
    Este archivo implementa una clase llamada JSON_DB.
    Basicamente maneja una base de datos simple guardada en formato JSON.
    Permite cargar, buscar, insertar, modificar y borrar objetos JSON en archivos
    como "db/products.json", "db/clients.json", etc.
*/


JSON_DB::JSON_DB(Alias path_alias)
    : path_alias_(path_alias),
      file_path_(resolve_path_(path_alias)),
      data_(json::array()) {
  try {
    load_(); // carga inicial del archivo JSON
  } catch (const std::exception& e) {
    std::cerr << "[JSON_DB::ctor] Error al cargar la base: " << e.what() << "\n";
    data_ = json::array();
  }
}

/*
    Esta función convierte el alias (por ejemplo, Alias::Products)
    - Si la ruta es absoluta (por ejemplo "C:/tienda/db/products.json"), la devuelve tal cual.
    - Si es relativa (por ejemplo "db/products.json"), la combina con el directorio actual.
    - Si ocurre algún error al obtener el directorio, lo informa y devuelve la ruta normalizada.
*/
std::filesystem::path JSON_DB::resolve_path_(Alias path_alias) {
  // Convierte el alias en una ruta, por ejemplo "db/products.json"
  std::filesystem::path resolved{std::string(to_path(path_alias))};

  // Si ya es una ruta absoluta, se devuelve directamente
  if (resolved.is_absolute()) {
    return resolved.lexically_normal();
  }

  // Si es relativa, se combmina con el directorio actual
  std::error_code ec;
  const auto base = std::filesystem::current_path(ec);

  // Si hubo error al obtener el directorio actual, se informa y devuelve la ruta base
  if (ec) {
    std::cerr << "[JSON_DB::resolve_path_] Error al obtener el directorio actual: "
              << ec.message() << std::endl;
    return resolved.lexically_normal();
  }

  // Devuelve la ruta completa y normalizada
  return (base / resolved).lexically_normal();
}

/*
    Funcion que se encarga de la carga del contenido del archivo JSON en memoria.
    - Si el archivo no existe, muestra un aviso y crea una base vacía.
    - Si el archivo existe pero no tiene el formato correcto (debe ser un array), también se reinicia vacío.
    - Si ocurre algún error al leer o parsear el archivo, se informa en consola.
    - Nunca lanza excepciones: siempre deja la base en un estado seguro (array vacío).
*/
void JSON_DB::load_() {
  try {
    // Abre el archivo JSON según la ruta que se guardó en file_path_
    std::ifstream input(file_path_);

    // Si el archivo no existe o no se puede abrir, se crea una base nueva en blanco
    if (!input.is_open()) {
      std::cerr << "[JSON_DB::load_] Archivo no encontrado. Se creará uno nuevo.\n";
      data_ = json::array();
      return;
    }
    // Intenta leer y parsear el contenido del archivo
    json parsed;
    input >> parsed;

    // Verifica que la raíz del JSON sea un array, como se espera (lista de objetos)
    if (!parsed.is_array()) {
      std::cerr << "[JSON_DB::load_] El JSON no contiene un array como raíz. Se reinicia vacío.\n";
      data_ = json::array();
      return;
    }

    // Si todo salió bien, guarda los datos en memoria
    data_ = std::move(parsed);
  } 
  catch (const json::parse_error& e) {
    // Error específico al leer JSON (por ejemplo, archivo corrupto o mal formado)
    std::cerr << "[JSON_DB::load_] Error al parsear JSON (" 
              << file_path_.string() << "): " << e.what() << "\n";
    data_ = json::array();
  } 
  catch (const std::exception& e) {
    // Cualquier otro tipo de error inesperado
    std::cerr << "[JSON_DB::load_] Excepción general: " << e.what() << "\n";
    data_ = json::array();
  }
}

/*
    Esta funcion guarda en disco el contenido actual de la base de datos (data_).

    - Crea el directorio si no existe (por ejemplo "db/").
    - Escribe los datos en un archivo temporal (.tmp) para evitar perder información
      si ocurre un error durante la escritura.
    - Luego reemplaza el archivo original por el temporal (operación segura).
    - Si algo falla (permisos, escritura, renombrado), muestra el error en consola y devuelve false.
    - Si todo sale bien, devuelve true.
*/
bool JSON_DB::save_() {
  try {
    // Verificar que el directorio donde se guardará el archivo exista
    const auto parent = file_path_.parent_path();
    if (!parent.empty()) {
      std::error_code ec;
      std::filesystem::create_directories(parent, ec); // crea "db/" si no existe
      if (ec) {
        std::cerr << "[JSON_DB::save_] No se pudo crear el directorio "
                  << parent.string() << ": " << ec.message() << "\n";
        return false;
      }
    }

    // Arma la ruta temporal donde primero se escribirá el contenido
    auto temp_path = file_path_;
    temp_path += ".tmp";

    // Escribe los datos JSON al archivo temporal
    {
      std::ofstream tmp(temp_path, std::ios::binary | std::ios::trunc);
      if (!tmp.is_open()) {
        std::cerr << "[JSON_DB::save_] No se pudo abrir el archivo temporal: "
                  << temp_path.string() << "\n";
        return false;
      }

      // Guarda los datos con indentación de 2 espacios (más legible)
      tmp << std::setw(2) << data_;

      if (!tmp.good()) {
        std::cerr << "[JSON_DB::save_] Error al escribir los datos en disco.\n";
        return false;
      }

      tmp.flush(); // fuerza a escribir todo el contenido
    }

    // Reemplaza el archivo original por el temporal (operación atómica)
    std::error_code ec;
    std::filesystem::rename(temp_path, file_path_, ec);

    // En algunos sistemas (como Windows) puede fallar si el archivo ya existe
    if (ec) {
      std::filesystem::remove(file_path_, ec); // se intenta borrarlo primero
      ec.clear();
      std::filesystem::rename(temp_path, file_path_, ec);

      if (ec) {
        std::cerr << "[JSON_DB::save_] No se pudo reemplazar el archivo original: "
                  << ec.message() << "\n";
        return false;
      }
    }

    // Si hasta aca no tiro ningun error, el guardado fue exitoso
    return true;
  } 
  catch (const std::exception& e) {
    // Cualquier error inesperado durante el guardado
    std::cerr << "[JSON_DB::save_] Excepción al guardar: " << e.what() << "\n";
    return false;
  }
}


/*
    Busca el índice dentro del array donde el campo "id" coincide
    con el ID dado. Devuelve std::nullopt si no lo encuentra.
*/
std::optional<std::size_t> JSON_DB::index_of_id_(const std::string& id) const {
  try {
    for (std::size_t i = 0; i < data_.size(); ++i) {
      const auto& item = data_[i];
      auto it = item.find("id");
      if (it == item.end()) continue;

      // Acepta tanto "id" en texto como numérico
      if (it->is_string() && *it == id) return i;
      if (it->is_number() && std::to_string(it->get<long long>()) == id) return i;
    }
  } catch (const std::exception& e) {
    std::cerr << "[JSON_DB::index_of_id_] Excepción: " << e.what() << "\n";
  }
  return std::nullopt;
}

/*
    Devuelve todo el contenido actual de la base (solo lectura).
*/
const json& JSON_DB::all() const {
  return data_;
}

/*
    Busca un objeto por su ID.
    Si lo encuentra, devuelve el JSON. Si no, devuelve std::nullopt.
*/
std::optional<json> JSON_DB::find_by_id(const std::string& id) const {
  try {
    if (auto idx = index_of_id_(id)) {
      return std::optional<json>(data_[*idx]);
    }
  } catch (const std::exception& e) {
    std::cerr << "[JSON_DB::find_by_id] Excepción: " << e.what() << "\n";
  }

  return std::nullopt;
}

/*
    Inserta un nuevo objeto en la base.
    El objeto debe tener un campo "id" (string). Si el ID ya existe, no lo inserta.
    Devuelve true si todo salió bien, false si algo falló.
*/
bool JSON_DB::insert(json obj) {
  try {
    if (!obj.contains("id")) {
      std::cerr << "[JSON_DB::insert] Falta el campo 'id'.\n";
      return false;
    }

    const json& id_field = obj["id"];
    if (!id_field.is_string()) {
      std::cerr << "[JSON_DB::insert] El campo 'id' debe ser string.\n";
      return false;
    }

    const std::string id = id_field.get<std::string>();
    if (index_of_id_(id)) {
      std::cerr << "[JSON_DB::insert] ID duplicado: " << id << "\n";
      return false;
    }

    data_.push_back(std::move(obj));
    if (!save_()) {
      std::cerr << "[JSON_DB::insert] Error al guardar los cambios.\n";
      return false;
    }

    return true;
  } catch (const std::exception& e) {
    std::cerr << "[JSON_DB::insert] Excepción: " << e.what() << "\n";
    return false;
  }
}

/*
    Actualiza un objeto existente buscándolo por su ID.
    Si no existe, informa el error. Si existe, lo reemplaza completamente.
*/
bool JSON_DB::update_by_id(json obj) {
  try {
    if (!obj.contains("id")) {
      std::cerr << "[JSON_DB::update_by_id] Falta el campo 'id'.\n";
      return false;
    }

    const json& id_field = obj["id"];
    if (!id_field.is_string()) {
      std::cerr << "[JSON_DB::update_by_id] El campo 'id' debe ser string.\n";
      return false;
    }

    const std::string id = id_field.get<std::string>();
    if (auto idx = index_of_id_(id)) {
      data_[*idx] = std::move(obj);
      if (!save_()) {
        std::cerr << "[JSON_DB::update_by_id] Error al guardar los cambios.\n";
        return false;
      }
      return true;
    }

    std::cerr << "[JSON_DB::update_by_id] No se encontró el item con id: " << id << "\n";
    return false;
  } catch (const std::exception& e) {
    std::cerr << "[JSON_DB::update_by_id] Excepción: " << e.what() << "\n";
    return false;
  }
}

/*
    Borra un objeto de la base según su ID.
    Si no se encuentra, devuelve false.
*/
bool JSON_DB::delete_by_id(const std::string& id) {
  try {
    if (auto idx = index_of_id_(id)) {
      data_.erase(data_.begin() + static_cast<std::ptrdiff_t>(*idx));
      if (!save_()) {
        std::cerr << "[JSON_DB::delete_by_id] Error al guardar los cambios.\n";
        return false;
      }
      return true;
    } else {
      std::cerr << "[JSON_DB::delete_by_id] No existe el id: " << id << "\n";
    }
  } catch (const std::exception& e) {
    std::cerr << "[JSON_DB::delete_by_id] Excepción: " << e.what() << "\n";
  }

  return false;
}
