/*
 * Copyright (c) 2021 Mike Castillo. All rights reserved.
 * Licensed under the MIT License. See the LICENSE file for full license information.
 *
 * Scanning Algorithms
 *
 * Algorithms for scanning sequences of elements, in the style of the C++ standard library.
 *
 * Part of the Programming Language Toolkit (PLTK).
 */

#pragma once

#include <algorithm>     // std::ranges::mismatch
#include <functional>    // std::invoke, std::identity
#include <iterator>


namespace pltk
{

template <class F, class I, class S, class... Args>
concept scanning_algorithm =
    std::input_iterator<I> &&
    std::sentinel_for<I, S> &&
    std::is_invocable_r_v<I, F, I, S, Args...>;


namespace detail
{

struct scan_fn
{
    template <std::input_iterator I, std::sentinel_for<I> S, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires std::indirect_binary_predicate<Pred, std::projected<I, Proj>, const T*>
    constexpr I operator() (I first, S last, const T& value, Pred pred = {}, Proj proj = {}) const
    {
        if (first == last || !std::invoke(pred, std::invoke(proj, *first), value))    return first;
        return first + 1;
    }


    template <std::ranges::input_range R, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires std::indirect_binary_predicate<Pred, std::projected<std::ranges::iterator_t<R>, Proj>, const T*>
    constexpr std::ranges::iterator_t<R> operator() (R&& r, const T& value, Pred pred = {}, Proj proj = {}) const
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), value, std::move(pred), std::move(proj));
    }


    // Note: std::forward_iterator is required for first1 so it remains valid if std::ranges::mismatch fails.
    template <std::forward_iterator I1, std::sentinel_for<I1> S1,
              std::input_iterator I2, std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr I1 operator() (
        I1 first1, S1 last1, I2 first2, S2 last2,
        Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}
    ) const
    {
        auto ends = std::ranges::mismatch(
            first1, last1, std::move(first2), last2,
            std::move(pred), std::move(proj1), std::move(proj2)
        );

        return ends.in2 == last2 ? ends.in1 : first1;
    }


    template <std::ranges::forward_range R, std::input_iterator I, std::sentinel_for<I> S,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R>, I, Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R>
    operator() (R&& r, I first, S last, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(
            std::ranges::begin(r), std::ranges::end(r), std::move(first), std::move(last),
            std::move(pred), std::move(proj1), std::move(proj2)
        );
    }


    template<std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I, std::ranges::iterator_t<R>, Pred, Proj1, Proj2>
    constexpr I operator() (I first, S last, R&& r, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(
            std::move(first), std::move(last),
            std::ranges::begin(r), std::ranges::end(r),
            std::move(pred), std::move(proj1), std::move(proj2)
        );
    }


    template<std::ranges::forward_range R1, std::ranges::input_range R2,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R1>, std::ranges::iterator_t<R2>,
                                            Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R1>
    operator() (R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(
            std::ranges::begin(r1), std::ranges::end(r1),
            std::ranges::begin(r2), std::ranges::end(r2),
            std::move(pred), std::move(proj1), std::move(proj2)
        );
    }
}; // struct scan_fn


struct scan_if_fn
{
    template <std::input_iterator I, std::sentinel_for<I> S,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::projected<I, Proj>> Pred>
    constexpr I operator() (I first, S last, Pred pred, Proj proj = {}) const
    {
        if (first == last || !std::invoke(pred, std::invoke(proj, *first)))    return first;
        return first + 1;
    }


    template <std::ranges::input_range R, class Proj = std::identity,
              std::indirect_unary_predicate<std::ranges::iterator_t<R>> Pred>
    constexpr std::ranges::iterator_t<R> operator() (R&& r, Pred pred, Proj proj = {}) const
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), std::move(pred), std::move(proj));
    }
}; // struct scan_if_fn


struct scan_not_fn
{
    template <std::input_iterator I, std::sentinel_for<I> S, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires std::indirect_binary_predicate<Pred, std::projected<I, Proj>, const T*>
    constexpr I operator() (I first, S last, const T& value, Pred pred = {}, Proj proj = {}) const
    {
        if (first == last || std::invoke(pred, std::invoke(proj, *first), value))    return first;
        return first + 1;
    }


    template <std::ranges::input_range R, class T,
              class Pred = std::ranges::equal_to, class Proj = std::identity>
        requires std::indirect_binary_predicate<Pred, std::projected<std::ranges::iterator_t<R>, Proj>, const T*>
    constexpr std::ranges::iterator_t<R> operator() (R&& r, const T& value, Pred pred = {}, Proj proj = {}) const
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), value, std::move(pred), std::move(proj));
    }


    // Note: std::forward_iterator is required for first1 so it remains valid after the call to std::ranges::mismatch.
    template <std::forward_iterator I1, std::sentinel_for<I1> S1,
              std::input_iterator I2, std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr I1 operator() (
        I1 first1, S1 last1, I2 first2, S2 last2,
        Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}
    ) const
    {
        auto ptrs = std::ranges::mismatch(
            first1, last1, std::move(first2), last2,
            std::move(pred), std::move(proj1), std::move(proj2)
        );

        return ptrs.in2 != last2 ? first1 + 1 : first1;
    }


    template <std::ranges::forward_range R, std::input_iterator I, std::sentinel_for<I> S,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R>, I, Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R>
    operator() (R&& r, I first, S last, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(
            std::ranges::begin(r), std::ranges::end(r), std::move(first), std::move(last),
            std::move(pred), std::move(proj1), std::move(proj2)
        );
    }


    template<std::forward_iterator I, std::sentinel_for<I> S, std::ranges::input_range R,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<I, std::ranges::iterator_t<R>, Pred, Proj1, Proj2>
    constexpr I operator() (I first, S last, R&& r, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(
            std::move(first), std::move(last),
            std::ranges::begin(r), std::ranges::end(r),
            std::move(pred), std::move(proj1), std::move(proj2)
        );
    }


    template<std::ranges::forward_range R1, std::ranges::input_range R2,
             class Pred = std::ranges::equal_to,
             class Proj1 = std::identity, class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R1>, std::ranges::iterator_t<R2>,
                                            Pred, Proj1, Proj2>
    constexpr std::ranges::iterator_t<R1>
    operator() (R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {}, Proj2 proj2 = {}) const
    {
        return operator()(
            std::ranges::begin(r1), std::ranges::end(r1),
            std::ranges::begin(r2), std::ranges::end(r2),
            std::move(pred), std::move(proj1), std::move(proj2)
        );
    }
}; // struct scan_not_fn


struct scan_if_not_fn
{
    template <std::input_iterator I, std::sentinel_for<I> S,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::projected<I, Proj>> Pred>
    constexpr I operator() (I first, S last, Pred pred, Proj proj = {}) const
    {
        if (first == last || std::invoke(pred, std::invoke(proj, *first)))    return first;
        return first + 1;
    }


    template <std::ranges::input_range R,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::ranges::iterator_t<R>> Pred>
    constexpr std::ranges::iterator_t<R> operator() (R&& r, Pred pred, Proj proj = {}) const
    {
        return operator()(std::ranges::begin(r), std::ranges::end(r), std::move(pred), std::move(proj));
    }
}; // struct scan_if_not_fn

} // namespace detail


inline constexpr detail::scan_fn        scan{};
inline constexpr detail::scan_if_fn     scan_if{};
inline constexpr detail::scan_not_fn    scan_not{};
inline constexpr detail::scan_if_not_fn scan_if_not{};


} // namespace pltk
