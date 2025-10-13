#ifndef DB_UTILS_H
#define DB_UTILS_H
#include <filesystem>
#include <optional>
#include <string>
#include <vector>
#include "single_include/nlohmann/json.hpp"
#include "db_enum_aliases.h"



using json = nlohmann::json;

class JSON_DB{
  /**
    DOCS:
    Clase que maneja la interaccion con los archivos JSON.

    Esta clase se encarga de que se instancie una unica vez por cada JSON 

    Esto permite no solo no sobrecargar el I/O del disco si no tambien evitar la corrupcion de datos.
  */


private:
  static std::filesystem::path resolve_path_(Alias path_alias);
  void load_();
  bool save_();

  Alias path_alias_;
  std::filesystem::path file_path_;
  json data_;

public:

  explicit JSON_DB(Alias path_alias);

  std::optional<std::size_t> index_of_id_(const std::string& id) const;

  bool insert(json obj);

  std::optional<json> find_by_id(const std::string& id) const;

  bool update_by_id(json obj);

  bool delete_by_id(const std::string& id);

  const json& all() const;

  std::vector<json> search_by_class_field(const std::string& field, const json& value) const;
};

#endif // DB_UTILS_H
