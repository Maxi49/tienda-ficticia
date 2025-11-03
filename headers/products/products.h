#pragma once
#include <optional>
#include <string>
#include <utility>
#include <unordered_map>

#include "../../headers/single_include/nlohmann/json.hpp"
using json = nlohmann::json;

#include "../../db_utils/db_enum_aliases.h"
#include "../../db_utils/json_db_utils.h"

/*
    Clase base abstracta Product
    Representa un producto genérico de la tienda (ya sea película o videojuego).
    Define los atributos y comportamientos comunes, como:
      ID, nombre, descripción, género, precio, stock.
      Métodos para alquilar y devolver productos.
      Métodos comunes para convertir el objeto a formato JSON (guardar en archivo).

    Las clases hijas (Movie y Game) heredan de esta y deben implementar
    sus propios métodos de tipo, mostrar información y convertir a JSON.
*/
class Product {
public:
    // Destructor virtual: permite eliminar correctamente objetos derivados (Movie, Game) desde punteros a Product
    virtual ~Product() = default;

    // Métodos que las clases hijas deben implementar 
    virtual void showInfo() const = 0;   // Muestra los datos del producto
    virtual std::string type() const = 0; // Devuelve el tipo (movie o game)
    virtual json to_json() const = 0;     // Convierte el objeto en formato JSON

    // Métodos para manejo de stock
    bool canRent(int amountReq) const;                       // Verifica si hay stock suficiente para alquilar
    bool applyRent(const std::string& client_id, int amountReq); // Aplica el alquiler (reduce stock y registra al cliente)
    bool applyReturn(const std::string& client_id, int amountReq); // Aplica la devolución (aumenta stock y actualiza registro)

    // Getters básicos
    int getId() const { return id; }
    const std::string& getName() const { return name; }

protected:
    /*
        Constructor base (solo puede ser usado por las clases hijas).
        Recibe los valores comunes de todos los productos.
    */
    Product(int id,
            const std::string& name,
            const std::string& genero,
            const std::string& description,
            float price,
            int totalStock);

    // Permite que las clases derivadas copien correctamente los datos comunes
    Product(const Product&) = default;
    Product& operator=(const Product&) = default;

    /*
        Convierte los datos comunes del producto a JSON.
        (Por ejemplo: id, nombre, género, precio, stock, etc.)
        Esto se usa cuando el producto se guarda en un archivo.
    */
    json base_json_() const;

    /*
        Hace lo inverso a base_json_: toma un JSON y carga sus valores dentro de un producto.
        Es estática porque no necesita un objeto creado previamente.
    */
    static void from_json_base(Product& p, const json& j);

protected:
    // Atributos comunes a todos los productos
    int id;                       
    std::string name;             
    std::string genero;           
    std::string description;      
    float price;                  
    int totalStock;               
    int availableStock;           
    bool rented;                  
    std::map<std::string, int> activeRentals; // guarda los alquileres activos: (id_cliente -> cantidad alquilada)
};

/*
    Sobrecarga del operador << para mostrar información rápida del producto.
    Permite imprimir el producto directamente con std::cout << producto;
    Ejemplo de salida:
      [3] Super Mario - Aventura $59.99 [game]
*/
inline std::ostream& operator<<(std::ostream& os, const Product& p) {
    const json j = p.to_json();
    os << "[" << j.value("id","?") << "] "
       << j.value("name","(sin nombre)") << " - "
       << j.value("genero","(sin genero)") << " $"
       << j.value("price", 0.0) << " [" << j.value("type","?") << "]";
    return os;
}
