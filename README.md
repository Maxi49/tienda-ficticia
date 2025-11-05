# Trabajo practico OOP: Tienda Ficticia
## Descripción General

Este proyecto implementa un **sistema de gestión de una tienda multimedia** desarrollado en C++17.  
Permite administrar de forma unificada el catálogo de productos (películas y videojuegos), clientes y transacciones (alquileres y devoluciones).  
El almacenamiento se realiza en formato **JSON persistente**, utilizando la librería `nlohmann/json` integrada en el proyecto.

El programa se ejecuta completamente en consola, con un menú interactivo para navegar entre las distintas secciones:
- Administración de productos.
- Administración de películas.
- Administración de videojuegos.
- Administración de clientes.

El objetivo del proyecto es modelar un sistema de stock y operaciones típico de un videoclub o tienda de medios, aplicando **conceptos de POO, persistencia, y manejo de datos JSON**.

---

## Estructura del Proyecto

```
.
├── db/                     # Bases de datos locales en formato JSON
│   ├── clients.json
│   ├── games.json
│   ├── movies.json
│   └── transactions.json
│
├── db_utils/               # Módulos para manipulación de archivos JSON
│   ├── db_enum_aliases.h
│   ├── json_db_utils.h
│   └── json_db_utils.cpp
│
├── headers/                # Archivos de cabecera principales
│   ├── client.h
│   ├── store.h
│   ├── transaction.h
│   └── products/           # Definiciones de clases de productos
│       ├── products.h
│       ├── game.h
│       └── movie.h
│
├── source/                 # Implementaciones de las clases
│   ├── client.cpp
│   ├── store.cpp
│   ├── transaction.cpp
│   └── products/
│       ├── product.cpp
│       ├── game.cpp
│       └── movie.cpp
│
├── utils/                  # Utilidades de entrada/salida y plantillas
│   ├── read_inputs.h
│   └── templates.h
│
├── single_include/nlohmann/ # Librería JSON embebida (no requiere instalación)
│   ├── json.hpp
│   └── json_fwd.hpp
│
├── main.cpp                # Punto de entrada del programa (menús y lógica principal)
├── CMakeLists.txt          # Configuración de build (opcional)
└── .gitignore

```

## Principales Componentes

### **1. Store**
Clase principal que gestiona:
- El catálogo de productos (`Game`, `Movie`).
- El registro de clientes.
- La carga y persistencia de datos en disco.
- Operaciones CRUD y consultas (búsqueda, filtrado, actualización de precio).

### **2. Product / Game / Movie**
- `Product` actúa como clase base común, con atributos compartidos (id, nombre, descripción, género, stock, precio).
- `Game` y `Movie` heredan de `Product` e implementan campos y serialización específicos.
- Cada clase define sus métodos `to_json()` y `from_json()` para compatibilidad con la base de datos JSON.

### **3. Client**
- Representa un cliente registrado.
- Contiene su identificador y nombre.
- Permite vincular las transacciones registradas.

### **4. TransactionService**
- Administra el proceso de alquiler y devolución de productos.
- Actualiza el stock disponible y registra los movimientos en `transactions.json`.

### **5. JSON_DB**
- Clase utilitaria que maneja la persistencia en archivos `.json`.
- Permite insertar, actualizar, eliminar y buscar objetos por ID.
- Implementa escritura segura con archivos temporales y verificación de consistencia.

### **6. Utilidades (`utils/`)**
- `read_inputs.h`: funciones auxiliares para lectura validada de datos desde consola.
- `templates.h`: implementaciones genéricas para `readValue<T>` y `showVectorInfo()`.

### **7. Menú Principal (main.cpp)**
- Proporciona una interfaz de usuario en consola mediante menús dinámicos.
- Cada sección (productos, películas, juegos, clientes) se encapsula en un submenú independiente.
- Se utiliza un sistema de acciones (`std::function`) para despachar las opciones seleccionadas.

---

## Requisitos

- **Compilador:** `g++` compatible con **C++17** o superior.
- **Sistema operativo:** Windows, Linux o macOS.
- **Editor recomendado:** [Visual Studio Code](https://code.visualstudio.com/docs/cpp/config-mingw#_prerequisites) con extensión `C/C++ (Microsoft)` y entorno `MinGW` o `MSYS2`.

---

## Compilación y Ejecución

Desde la raíz del proyecto, ejecutar el siguiente comando:
```
g++ -std=c++17 -I"./headers" -I"./db_utils" main.cpp source/products/game.cpp source/products/movie.cpp source/products/product.cpp source/client.cpp source/store.cpp source/transaction.cpp db_utils/json_db_utils.cpp -o tienda.exe

````

Luego, ejecutar el programa:

```
./tienda.exe
```

---

## Persistencia y Archivos JSON

Los datos se almacenan en el directorio `db/`:

* `clients.json`: registros de clientes.
* `games.json`: catálogo de videojuegos.
* `movies.json`: catálogo de películas.
* `transactions.json`: histórico de alquileres y devoluciones.

Cada archivo se gestiona a través de la clase `JSON_DB`, que garantiza:

* Inserción y actualización atómica.
* Escritura mediante archivo temporal `.tmp`.
* Recuperación segura ante errores de parseo.

---

## Ejemplo de Uso

**Flujo básico:**

1. Ejecutar el programa (`./tienda.exe`).
2. Seleccionar “Administrar Productos”.
3. Listar, buscar o alquilar un producto.
4. Actualizar su precio o devolverlo.
5. Salir del menú principal para persistir los datos.

**Estructura típica de consola:**

```
=== MENU PRINCIPAL ===
1) Administrar Productos
2) Administrar Películas
3) Administrar Juegos
4) Administrar Clientes
0) Salir (guardar)
```

---

## Buenas Prácticas Implementadas

* **Principio DRY (Don't Repeat Yourself):**
  Se centralizó la lógica de menús y acciones mediante funciones genéricas (`run_menu_loop`, `MenuItem`).

* **Manejo robusto de errores:**
  Cada operación de E/S se encuentra envuelta en bloques `try/catch`, reportando errores sin abortar el programa.

* **Persistencia desacoplada:**
  El sistema `JSON_DB` abstrae totalmente la interacción con el disco.

* **Diseño modular:**
  La separación en carpetas por responsabilidad facilita la extensión y mantenimiento del sistema.


