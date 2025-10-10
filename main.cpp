#include <iostream>
#include "headers/products/products.h"
#include "single_include/nlohmann/json.hpp"

using json = nlohmann::json;
int main() {
    std::cout << "Hola Maxi desde C++!" << std::endl;

    json persona = {
        {"name", "Maxi"},
        {"age", 22},
        {"city", "Buenos Aires"}
    };

    std::cout << persona.dump(4) << std::endl;

    return 0;
}
