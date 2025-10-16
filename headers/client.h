#pragma once

#include <iostream>
#include <string>

#include "single_include/nlohmann/json.hpp"
#include "../db_utils/db_enum_aliases.h"
#include "../db_utils/json_db_utils.h"

class Client {
private: 
    int clientID;
    std::string name;


public:
    //Constructor 
    Client(int id, const std::string& name);

    //Destructor de la clase
    ~Client();

    //Constructo de copia
    Client(const Client& other);

    //Operador de asigancion
    Client& operator=(const Client& other);

    //Getters
    int getID() const{return clientID;}
    const std::string& getName() const { return name; }    
    //Setters
    void setName(const std::string& newName);
    
    //Mostrar informacion
    void showInfo() const;
};
