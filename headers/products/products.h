#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <chrono>
#include <optional>
#include <cstddef>

class product {
public:
    using days     = std::chrono::days;
    using sys_days = std::chrono::sys_days;

private:
    std::string title{};
    std::string genre{};
    std::size_t quantity_available{};
    double rent_price{};
    std::optional<sys_days> start_rent_date{};
    std::optional<sys_days> end_rent_date{};
    std::optional<days>     rent_period{};
};

#endif // PRODUCT_H
