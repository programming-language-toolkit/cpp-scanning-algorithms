**************************************************************************************************************
Scanning Algorithms
**************************************************************************************************************

==============================================================================================================
scanning_algorithm
==============================================================================================================
Specifies the requirements of a function type that advances an iterator.


Synopsis
-------------------------------------------------------
::

    template <class F, class I, class S, class... Args>
    concept scanning_algorithm =
        std::input_iterator<I> &&
        std::sentinel_for<I, S> &&
        std::is_invocable_r_v<I, F, I, S, Args...>;


==============================================================================================================
scan, scan_if
==============================================================================================================
Advances an iterator only if the first elements in a range satisfy specific criteria.


Call signature
-------------------------------------------------------
1) ::

    template <std::input_iterator I, std::sentinel_for<I> S, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires std::indirect_binary_predicate<Pred, std::projected<I, Proj>, const T*>
    constexpr I scan (I first, S last, const T& value, Pred pred = {}, Proj proj = {})

Returns ``first + 1`` if the first element of the range is equal to ``value``. Comparison is done by applying ``pred`` to the first element projected by ``proj``.

2) ::

    template <std::ranges::input_range R, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires
            std::indirect_binary_predicate<Pred,
                                           std::projected<std::ranges::iterator_t<R>, Proj>,
                                           const T*>
    constexpr std::ranges::iterator_t<R>
    scan (R&& r, const T& value, Pred pred = {}, Proj proj = {})

Same as (1), using ``r`` as the source range.

3) ::

    template <std::forward_iterator I1, std::sentinel_for<I1> S1,
              std::input_iterator I2, std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr I1 scan (
        I1 first1, S1 last1, I2 first2, S2 last2,
        Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}
    )

Let N equal the size of the second range. Returns ``first1 + N`` if there are at least N elements in the first range and the first N elements are equal to the corresponding elements of the second range. Comparison is done by applying ``pred`` to the elements of each range projected by ``proj1`` and ``proj2`` respectively.


4) ::

    template <std::ranges::forward_range R, std::input_iterator I, std::sentinel_for<I> S,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R>, I, Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R>
    scan (R&& r, I first, S last, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {})

Same as (3), using ``r`` as the source range.

5) ::

    template<std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I, std::ranges::iterator_t<R>, Pred, Proj1, Proj2>
    constexpr I scan (I first, S last, R&& r, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {})

Same as (3), using ``r`` as the comparison range.

6) ::

    template<std::ranges::forward_range R1, std::ranges::input_range R2,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R1>,
                                            std::ranges::iterator_t<R2>,
                                            Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R1>
    scan (R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {})

Same as (3), using ``r1`` as the source range and ``r2`` as the comparison range.

7) ::

    template <std::input_iterator I, std::sentinel_for<I> S,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::projected<I, Proj>> Pred>
    constexpr I scan_if (I first, S last, Pred pred, Proj proj = {})

Returns ``first + 1`` if the first element of the range satisfies ``pred`` after projection by ``proj``.

8) ::

    template <std::ranges::input_range R, class Proj = std::identity,
              std::indirect_unary_predicate<std::ranges::iterator_t<R>> Pred>
    constexpr std::ranges::iterator_t<R> scan_if (R&& r, Pred pred, Proj proj = {})

Same as (7), using ``r`` as the source range.


Returns
-------------------------------------------------------
Iterator to the appropriate element if the condition was satisfied or to the first element if not.


Side effects
-------------------------------------------------------
None.


Complexity
-------------------------------------------------------
| 1, 2) O(1)
| 3-6) O(n), where n is the number of elements in ``[first2, last2)``
| 7, 8) O(1) + one invocation of ``pred`` and ``proj``


Examples
-------------------------------------------------------

::

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

Output

.. code-block:: text

     Single element: r
     Range of elements: u
     Predicate: s


==============================================================================================================
scan_not, scan_if_not
==============================================================================================================
Advances an iterator by one only if the first elements in a range do not satisfy specific criteria.


Synopsis
-------------------------------------------------------
1) ::

     template <std::input_iterator I, std::sentinel_for<I> S, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires std::indirect_binary_predicate<Pred, std::projected<I, Proj>, const T*>
    constexpr I scan_not (I first, S last, const T& value, Pred pred = {}, Proj proj = {})

Returns ``first + 1`` if the first element of the range is not equal to ``value``. Comparison is done by applying ``pred`` to the first element projected by ``proj``.

2) ::

    template <std::ranges::input_range R, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires
            std::indirect_binary_predicate<Pred,
                                           std::projected<std::ranges::iterator_t<R>, Proj>,
                                           const T*>
    constexpr std::ranges::iterator_t<R>
    scan_not (R&& r, const T& value, Pred pred = {}, Proj proj = {})

Same as (1), using ``r`` as the source range.

3) ::

     template <std::forward_iterator I1, std::sentinel_for<I1> S1,
              std::input_iterator I2, std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr I1 scan_not (
        I1 first1, S1 last1, I2 first2, S2 last2,
        Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}
    )

Let N equal the size of the second range. Returns ``first1 + 1`` if there are less than N elements in the first range or the first N elements are not equal to the corresponding elements of the second range. Comparison is done by applying ``pred`` to the elements of each range projected by ``proj1`` and ``proj2`` respectively.

4) ::

     template <std::ranges::forward_range R, std::input_iterator I, std::sentinel_for<I> S,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R>, I, Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R>
    scan_not (R&& r, I first, S last, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {})

Same as (3), using ``r`` as the source range.

5) ::

     template<std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I, std::ranges::iterator_t<R>, Pred, Proj1, Proj2>
    constexpr I
    scan_not (I first, S last, R&& r, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {})

Same as (3), using ``r`` as the comparison range.

6) ::

     template<std::ranges::forward_range R1, std::ranges::input_range R2,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R1>,
                                            std::ranges::iterator_t<R2>,
                                            Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R1>
    scan_not (R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {})

Same as (3), using ``r1`` as the source range and ``r2`` as the comparison range.

7) ::

    template <std::input_iterator I, std::sentinel_for<I> S,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::projected<I, Proj>> Pred>
    constexpr I scan_if_not (I first, S last, Pred pred, Proj proj = {})

Returns ``first + 1`` if the first element of the range does not satisfy ``pred`` after projection by ``proj``.

8) ::

    template <std::ranges::input_range R,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::ranges::iterator_t<R>> Pred>
    constexpr std::ranges::iterator_t<R> scan_if_not (R&& r, Pred pred, Proj proj = {})

Same as (7), using ``r`` as the source range.


Returns
-------------------------------------------------------
Iterator to the appropriate element if the condition was not satisfied or to the first element if so.


Side effects
-------------------------------------------------------
None.


Complexity
-------------------------------------------------------
| 1, 2) O(1)
| 3-6) O(n), where n is the number of elements in ``[first2, last2)``
| 7, 8) O(1) + one invocation of ``pred`` and ``proj``


Examples
-------------------------------------------------------

::

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

Output

.. code-block:: text

     Single element: a
     Range of elements: l
     Predicate: k
