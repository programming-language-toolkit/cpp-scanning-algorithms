#include <cctype>          // std::toupper, std::tolower
#include <cstring>         // std::strlen
#include <functional>      // std::reference_wrapper
#include <string>
#include <string_view>
#include <type_traits>     // std::is_invocable

#include "catch2/catch.hpp"
#include "scanning_algorithms.hpp"

using namespace pltk;


// =====================================================================================================================
// scan
// =====================================================================================================================
SCENARIO("The scan function should have a certain signature.")
{
    GIVEN("in the front position an iterator / sentinel pair")
    {
        GIVEN("in the back position a value, an iterator / sentinel pair, and a range")
        {
            THEN("the scan function can be called with each signature and should return an iterator.")
            {
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan),
                        std::string::iterator, std::string::iterator, std::string::iterator, std::string::iterator>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan),
                        std::string::iterator, std::string::iterator, std::string::value_type>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan),
                        std::string::iterator, std::string::iterator, std::string>);
            }
        }
    }

    GIVEN("in the front position a range")
    {
        GIVEN("in the back position a value, an iterator / sentinel pair, and a range")
        {
            THEN("the scan function can be called with each signature and should return an iterator.")
            {
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan), std::string, std::string>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan),
                        std::string, std::string::value_type>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan),
                        std::string, std::string::iterator, std::string::iterator>);
            }
        }
    }
}

SCENARIO("The scan function should advance its return iterator by a certain amount.")
{
    GIVEN("an iterator / sentinel pair and a source range")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        WHEN("the scan function is called with the source and a value")
        {
            THEN("it should advance the iterator by one if the source begins with the value.")
            {
                REQUIRE(first + 1 == scan(first, last, 'H'));
                REQUIRE(first + 1 == scan(source, 'H'));
            }

            THEN("it should not advance the iterator if the source does not begin with the value.")
            {
                REQUIRE(first == scan(first, last, 'B'));
                REQUIRE(first == scan(source, 'B'));
            }
        }

        WHEN("the scan function is called with the source and another range")
        {
            THEN("it should advance the iterator if the source begins with the comparison range by the length of that "
                 "range.")
            {
                std::string_view hello = "Hello";

                REQUIRE(first + 5 == scan(first, last, hello.begin(), hello.end()));
                REQUIRE(first + 5 == scan(source, hello));
            }

            THEN ("it should not advance the iterator if the source does not begin with the comparison range.")
            {
                std::string_view bye = "Bye";

                REQUIRE(first == scan(first, last, bye.begin(), bye.end()));
                REQUIRE(first == scan(source, bye));
            }
        }

        WHEN("the scan function is called with the source and a predicate")
        {
            auto returns_true  = [] (char, char) { return true; };
            auto returns_false = [] (char, char) { return false; };

            WHEN("the source is compared with a value")
            {
                THEN("it should advance the iterator by one if the predicate returns true even if it is not equal.")
                {
                    REQUIRE(first + 1 == scan(first, last, 'B', returns_true));
                    REQUIRE(first + 1 == scan(source, 'B', returns_true));
                }

                THEN("it should not advance the iterator if the predicate returns false even if it is equal.")
                {
                    REQUIRE(first == scan(first, last, 'H', returns_false));
                    REQUIRE(first == scan(source, 'H', returns_false));
                }
            }

            WHEN("the source is compared with a range")
            {
                THEN("it should advance the iterator if the predicate returns true over the extent of the range by the "
                     "length of the range even if it is not equal.")
                {
                    std::string_view bye = "Bye";

                    REQUIRE(first + 3 == scan(first, last, bye.begin(), bye.end(), returns_true));
                    REQUIRE(first + 3 == scan(source, bye, returns_true));
                }

                THEN("it should not advance the iterator if the predicate returns false over the extent of the range "
                     "even if it is equal.")
                {
                    std::string_view hello = "Hello";

                    REQUIRE(first == scan(first, last, hello.begin(), hello.end(), returns_false));
                    REQUIRE(first == scan(source, hello, returns_false));
                }
            }
        }
    }
}

SCENARIO("The scan function should take a comparision predicate.")
{
    GIVEN("an iterator / sentinel pair, a source range, and some predicates")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto returns_true  = [] (char, char) { return true; };
        auto returns_false = [] (char, char) { return false; };

        WHEN("the scan function is called with the source and a predicate")
        {
            THEN("it should advance the iterator if the predicate returns true given a value even if it is not equal.")
            {
                REQUIRE(first != scan(first, last, 'B', returns_true));
                REQUIRE(first != scan(source, 'B', returns_true));
            }

            THEN("it should not advance the iterator if the predicate returns false given a value even if it is equal.")
            {
                REQUIRE(first == scan(first, last, 'H', returns_false));
                REQUIRE(first == scan(source, 'H', returns_false));
            }

            THEN("it should advance the iterator if the predicate returns true over the extent of a comparison range "
                 "even if it is not equal.")
            {
                std::string_view bye = "Bye";

                REQUIRE(first != scan(first, last, bye.begin(), bye.end(), returns_true));
                REQUIRE(first != scan(source, bye, returns_true));
            }

            THEN("it should not advance the iterator if the predicate returns false over the extent of a comparision "
                 "range even if it is equal.")
            {
                std::string_view hello = "Hello";

                REQUIRE(first == scan(first, last, hello.begin(), hello.end(), returns_false));
                REQUIRE(first == scan(source, hello, returns_false));
            }
        }
    }
}

SCENARIO("The scan function should take a projection.")
{
    GIVEN("an iterator / sentinel pair, a source range, and some projections")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto to_upper = [] (unsigned char c) { return std::toupper(c); };
        auto to_lower = [] (unsigned char c) { return std::tolower(c); };

        WHEN("the source range is compared with a value after applying some projection")
        {
            THEN("the scan function should advance the iterator if the source begins with the projected result.")
            {
                REQUIRE(first != scan(first, last, 'h', {}, to_lower));
                REQUIRE(first != scan(source, 'h', {}, to_lower));
            }

            THEN("the scan function should not advance the iterator if the source does not begin with the projected "
                 "result.")
            {
                REQUIRE(first == scan(first, last, 'H', {}, to_lower));
                REQUIRE(first == scan(source, 'H', {}, to_lower));
            }
        }

        WHEN("the source range is compared with a range after applying some projection")
        {
            THEN("the scan function should advance the iterator if the source begins with the projected result.")
            {
                std::string_view hello = "hello";

                REQUIRE(first != scan(first, last, hello.begin(), hello.end(), {}, to_upper, to_upper));
                REQUIRE(first != scan(source, hello, {}, to_upper, to_upper));
            }

            THEN("the scan function should not advance the iterator if the source does not begin with the projected "
                 "result.")
            {
                std::string_view hello = "HELLO";

                REQUIRE(first == scan(first, last, hello.begin(), hello.end(), {}, to_upper, to_lower));
                REQUIRE(first == scan(source, hello, {}, to_upper, to_lower));
            }
        }
    }
}


// =====================================================================================================================
// scan_if
// =====================================================================================================================
SCENARIO("The scan_if function should have a certain signature.")
{
    GIVEN("an iterator / sentinel pair, a range, and a predicate")
    {
        auto returns_true = [] (char) { return true; };

        THEN("the scan_if function can be called with each signature and should return an iterator");
        {
            REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_if),
                    std::string::iterator, std::string::iterator, decltype(returns_true)>);
            REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_if),
                    std::string, decltype(returns_true)>);
        }
    }
}

SCENARIO("The scan_if function should advance its return iterator by a certain amount.")
{
    GIVEN("an iterator / sentinel pair, a source range, and some predicates")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto returns_true  = [] (char) { return true; };
        auto returns_false = [] (char) { return false; };

        WHEN("the scan_if function is called with the source and predicate")
        {
            THEN("it should advance the iterator by one if the predicate returns true.")
            {
                REQUIRE(first + 1 == scan_if(first, last, returns_true));
                REQUIRE(first + 1 == scan_if(source, returns_true));
            }

            THEN("it should not advance the iterator if the predicate returns false.")
            {
                REQUIRE(first == scan_if(first, last, returns_false));
                REQUIRE(first == scan_if(source, returns_false));
            }
        }
    }
}

SCENARIO("The scan_if function should take a projection.")
{
    GIVEN("an iterator / sentinel pair, a source range, and a projection")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto to_lower   = [] (unsigned char c) { return std::tolower(c); };
        auto is_H_upper = [] (char c) { return c == 'H'; };
        auto is_h_lower = [] (char c) { return c == 'h'; };

        WHEN("the predicate is called after applying some projection")
        {
            THEN("the scan_if function should advance the iterator if the predicate returns true with the projected "
                 "result.")
            {
                REQUIRE(first != scan_if(first, last, is_h_lower, to_lower));
                REQUIRE(first != scan_if(source, is_h_lower, to_lower));
            }

            THEN("the scan_if function should not advance the iterator if the predicate returns false with the "
                 "projected result.")
            {
                REQUIRE(first == scan_if(first, last, is_H_upper, to_lower));
                REQUIRE(first == scan_if(source, is_H_upper, to_lower));
            }
        }
    }
}


// =====================================================================================================================
// scan_not
// =====================================================================================================================
SCENARIO("The scan_not function should have a certain signature.")
{
    GIVEN("in the front position an iterator / sentinel pair")
    {
        GIVEN("in the back position a value, an iterator / sentinel pair, and a range")
        {
            THEN("the scan_not function can be called with each signature and should return an iterator.")
            {
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_not),
                        std::string::iterator, std::string::iterator, std::string::iterator, std::string::iterator>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_not),
                        std::string::iterator, std::string::iterator, std::string::value_type>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_not),
                        std::string::iterator, std::string::iterator, std::string>);
            }
        }
    }

    GIVEN("in the front position a range")
    {
        GIVEN("in the back position a value, an iterator / sentinel pair, and a range")
        {
            THEN("the scan_not function can be called with each signature and should return an iterator.")
            {
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_not), std::string, std::string>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_not),
                        std::string, std::string::value_type>);
                REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_not),
                        std::string, std::string::iterator, std::string::iterator>);
            }
        }
    }
}

SCENARIO("The scan_not function should advance its return iterator by a certain amount.")
{
    GIVEN("an iterator / sentinel pair and a mutable source range")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        WHEN("the scan_not function is called with the source and a value")
        {
            THEN("it should advance the iterator by one if the source does not begin with the value.")
            {
                REQUIRE(first + 1 == scan_not(first, last, 'B'));
                REQUIRE(first + 1 == scan_not(source, 'B'));
            }

            THEN("it should not advance the iterator if the source begins with the value.")
            {
                REQUIRE(first == scan_not(first, last, 'H'));
                REQUIRE(first == scan_not(source, 'H'));
            }
        }

        WHEN("the scan_not function is called with the source and another range")
        {
            THEN("it should not advance the iterator if the source begins with the comparison range.")
            {
                std::string_view hello = "Hello";

                REQUIRE(first == scan_not(first, last, hello.begin(), hello.end()));
                REQUIRE(first == scan_not(source, hello));
            }

            THEN("it should advance the iterator by one if the source does not begin with the comparison range.")
            {
                std::string_view bye = "Bye";

                REQUIRE(first + 1 == scan_not(first, last, bye.begin(), bye.end()));
                REQUIRE(first + 1 == scan_not(source, bye));
            }
        }

        WHEN("the scan_not function is called with the source and a predicate")
        {
            auto returns_true  = [] (char, char) { return true; };
            auto returns_false = [] (char, char) { return false; };

            WHEN("the source is compared with a value")
            {
                THEN("it should not advance the iterator if the predicate returns true even if it is not equal.")
                {
                    REQUIRE(first == scan_not(first, last, 'B', returns_true));
                    REQUIRE(first == scan_not(source, 'B', returns_true));
                }

                THEN("it should advance the iterator by one if the predicate returns false even if it is equal.")
                {
                    REQUIRE(first + 1 == scan_not(first, last, 'H', returns_false));
                    REQUIRE(first + 1 == scan_not(source, 'H', returns_false));
                }
            }

            WHEN("the source is compared with a range")
            {
                THEN("it should not advance the iterator if the predicate returns true over the extent of the range "
                     "even if it is not equal.")
                {
                    std::string_view bye = "Bye";

                    REQUIRE(first == scan_not(first, last, bye.begin(), bye.end(), returns_true));
                    REQUIRE(first == scan_not(source, bye, returns_true));
                }

                THEN("it should advance the iterator by one if the predicate returns false even if it is equal.")
                {
                    std::string_view hello = "Hello";

                    REQUIRE(first + 1 == scan_not(first, last, hello.begin(), hello.end(), returns_false));
                    REQUIRE(first + 1 == scan_not(source, hello, returns_false));
                }
            }
        }
    }
}

SCENARIO("The scan_not function should take a comparision predicate.")
{
    GIVEN("an iterator / sentinel pair, a source range, and some predicates")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto returns_true  = [] (char, char) { return true; };
        auto returns_false = [] (char, char) { return false; };

        WHEN("the scan_not function is called with the source and a predicate")
        {
            THEN("it should not advance the iterator if the predicate returns true given a value even if it is not "
                 "equal.")
            {
                REQUIRE(first == scan_not(first, last, 'B', returns_true));
                REQUIRE(first == scan_not(source, 'B', returns_true));
            }

            THEN("it should advance the iterator if the predicate returns false given a value even if it is equal.")
            {
                REQUIRE(first != scan_not(first, last, 'H', returns_false));
                REQUIRE(first != scan_not(source, 'H', returns_false));
            }

            THEN("it should not advance the iterator if the predicate returns true over the extent of a comparison "
                 "range even if it is not equal.")
            {
                std::string_view bye = "Bye";

                REQUIRE(first == scan_not(first, last, bye.begin(), bye.end(), returns_true));
                REQUIRE(first == scan_not(source, bye, returns_true));
            }

            THEN("it should advance the iterator if the predicate returns false over the extent of a comparison range "
                 "even if it is equal.")
            {
                std::string_view hello = "Hello";

                REQUIRE(first != scan_not(first, last, hello.begin(), hello.end(), returns_false));
                REQUIRE(first != scan_not(source, hello, returns_false));
            }
        }
    }
}

SCENARIO("The scan_not function should take a projection.")
{
    GIVEN("an iterator / sentinel pair, a source range, and some projections")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto to_upper = [] (unsigned char c) { return std::toupper(c); };
        auto to_lower = [] (unsigned char c) { return std::tolower(c); };

        WHEN("the source range is compared with a value after applying some projection")
        {
            THEN("the scan_not function should not advance the iterator if the source begins with the projected "
                 "result.")
            {
                REQUIRE(first == scan_not(first, last, 'h', {}, to_lower));
                REQUIRE(first == scan_not(source, 'h', {}, to_lower));
            }

            THEN("the scan_not function should advance the iterator if the source does not begin with the projected "
                 "result.")
            {
                REQUIRE(first != scan_not(first, last, 'H', {}, to_lower));
                REQUIRE(first != scan_not(source, 'H', {}, to_lower));
            }
        }

        WHEN("the source range is compared with a range after applying some projection")
        {
            THEN("the scan_not function should not advance the iterator if the source begins with the projected "
                 "result.")
            {
                std::string_view hello = "Hello";

                REQUIRE(first == scan_not(first, last, hello.begin(), hello.end(), {}, to_upper, to_upper));
                REQUIRE(first == scan_not(source, hello, {}, to_upper, to_upper));
            }

            THEN("the scan_not function should advance the iterator if the source does not begin with the projected "
                 "result.")
            {
                std::string_view hello = "HELLO";

                REQUIRE(first != scan_not(first, last, hello.begin(), hello.end(), {}, to_upper, to_lower));
                REQUIRE(first != scan_not(source, hello, {}, to_upper, to_lower));
            }
        }
    }
}


// =====================================================================================================================
// scan_if_not
// =====================================================================================================================
SCENARIO("The scan_if_not function should have a certain signature.")
{
    GIVEN("an iterator / sentinel pair, a range, and a predicate.")
    {
        auto returns_true = [] (char) { return true; };

        THEN("the scan_if_not function can be called with each signature and should return an iterator.")
        {
            REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_if_not),
                    std::string::iterator, std::string::iterator, decltype(returns_true)>);
            REQUIRE(std::is_invocable_r_v<std::string::iterator, decltype(scan_if_not),
                    std::string, decltype(returns_true)>);
        }
    }
}

SCENARIO("The scan_if_not function should advance its return iterator by a certain amount.")
{
    GIVEN("an iterator / sentienl pair, a source range, and some predicates")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto returns_true  = [] (char) { return true; };
        auto returns_false = [] (char) { return false; };

        WHEN("the scan_if_not function is called with the source and predicate")
        {
            THEN("it should advance the iterator by one if the predicate returns false.")
            {
                REQUIRE(first + 1 == scan_if_not(first, last, returns_false));
                REQUIRE(first + 1 == scan_if_not(source, returns_false));
            }

            THEN("it should not advance the iterator if the predicate returns true.")
            {
                REQUIRE(first == scan_if_not(first, last, returns_true));
                REQUIRE(first == scan_if_not(source, returns_true));
            }
        }
    }
}

SCENARIO("The scan_if_not function should take a projection.")
{
    GIVEN("an iterator / sentinel pair, a source range, and a projection")
    {
        std::string_view source = "Hello, world!";
        auto first = source.begin();
        auto last  = source.end();

        auto to_lower   = [] (unsigned char c) { return std::tolower(c); };
        auto is_H_upper = [] (char c) { return c == 'H'; };
        auto is_h_lower = [] (char c) { return c == 'h'; };

        WHEN("the predicate is called after applying some projection")
        {
            THEN("the scan_if_not function should not advance the iterator if the predicate returns true with the "
                 "projected result.")
            {
                REQUIRE(first == scan_if_not(first, last, is_h_lower, to_lower));
                REQUIRE(first == scan_if_not(source, is_h_lower, to_lower));
            }

            THEN("the scan_if_not function should advance the iterator if the predicate returns false with the "
                 "projected result.")
            {
                REQUIRE(first != scan_if_not(first, last, is_H_upper, to_lower));
                REQUIRE(first != scan_if_not(source, is_H_upper, to_lower));
            }
        }
    }
}
