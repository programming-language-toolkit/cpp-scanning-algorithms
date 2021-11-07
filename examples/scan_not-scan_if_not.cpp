#include <iostream>
#include <string_view>
#include "scanning_algorithms.hpp"

using namespace std::literals;
using namespace pltk;

int main ()
{
    std::string_view source = "Talk is cheap. Show me the code. -- Linus Torvalds";

    auto first = source.begin();
    auto last  = source.end();

    first = scan_not(first, last, 'Q');
    std::cout << "Single element: " << *first << '\n';

    first = scan_not(first, last, "alks"sv);
    std::cout << "Range of elements: " << *first << '\n';

    first = scan_if_not(first, last, [] (char c) { return c == 'f'; });
    std::cout << "Predicate: " << *first << '\n';
}