#include <iostream>
#include <string>
using namespace std;

//creacion de las clases principales
class Producto {
public:
    int id, idCliente;
    string titulo, genero;
    float precio;
    bool alquilado;

    Producto(int id, int idCLiente, string titulo, string genero, float precio, bool alquilado):
        id(id),
        idCliente(idCLiente),
        titulo(titulo),
        genero(genero),
        precio(precio),
        alquilado(alquilado){
    }

};

class Videojuego : public Producto{
public:
    string plataforma;
    int cantJugadores;

    Videojuego(int id, int idCLiente, string titulo, string genero, string plataforma, int cantJugadores, float precio, bool alquilado)
            :Producto(id,idCliente, titulo, genero, precio, alquilado), plataforma(plataforma), cantJugadores(cantJugadores) {
    }
    void mostrarVideoJuego(){
    cout << "id" << "idCliente" << "Titulo" << "Genero" << "Plataforma" << "Cantidad Jugadores" << "Precio" << "Alquilado" << endl;
    cout << "|" << id << "|" << idCliente << "|" << titulo << "|" << genero << "|" << plataforma << "|" << cantJugadores << "|" << precio << "|" << alquilado << "|" << endl;

    }
};


class Pelicula : public Producto{
public:
    string director;
    float duracion;

    Pelicula(int id, int idCLiente, string titulo, string genero, string direcotr, int duracion, float precio, bool alquilado)
            :Producto(id,idCliente, titulo, genero, precio, alquilado), director(director), duracion(duracion) {
    }
    void mostrarPelicula(){
        cout << "id" << "idCliente" << "Titulo" << "Genero" << "Director" << "duracion" << "Precio" << "Alquilado" << endl;
        cout << "|" << id << "|" << idCliente << "|" << titulo << "|" << genero << "|" << director << "|" << duracion << "|" << precio << "|" << alquilado << "|" << endl;

    }
};
int main() {
    Producto p1(0,"God of War", "Accion", 10.00, false,NULL);
    Producto p2(1, "Terminator", "Accion", 5.50, true, 1);
    Videojuego v1(0, "God of War", "Accion", 10.00, false, "xbox", "1");
    Pelicula c1(1, "Terminator", "Accion", 5.50, true, "Esteban", 160);
    cout << "-----------INFO PRODUCTOS------------ \n";

    cout << "\n\n";
    cout << "------------INFO JUEGOS------------\n";
    v1.mostrarVideoJuego();

    cout << "\n\n";
    cout << "------------INFO PELICULAS------------\n";
    c1.mostrarPelicula();
    return 0;
}
