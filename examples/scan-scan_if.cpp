#include <iostream>
#include <string_view>
#include "scanning_algorithms.hpp"

using namespace std::literals;
using namespace pltk;

int main ()
{
    std::string_view source = "Programs must be written for people to read, "
                              "and only incidentally for machines to execute. "
                              "-- Harold Abelson";

    auto first = source.begin();
    auto last  = source.end();

    first = scan(first, last, 'P');
    std::cout << "Single element: " << *first << '\n';

    first = scan(first, last, "rograms m"sv);
    std::cout << "Range of elements: " << *first << '\n';

    first = scan_if(first, last, [] (char c) { return c == 'u'; });
    std::cout << "Predicate: " << *first << '\n';
}