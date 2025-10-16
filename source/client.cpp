#include "../headers/client.h"
#include <iostream>

// Constructor
Client::Client(int id, const std::string& name)
    : clientID(id), name(name) {}

//Destructor
Client :: ~Client(){}

//Contructor copia
Client::Client(const Client& other) 
    : clientID(other.clientID), 
      name(other.name) {}

// Operador de asigancion
Client& Client::operator=(const Client& other) {
    if (this != &other) {
        clientID = other.clientID;
        name = other.name;
    }
    return *this;
}

// Salida de información
void Client::showInfo() const {
    std::cout << "Cliente ID: " << clientID << " | Nombre: " << name << std::endl;
}