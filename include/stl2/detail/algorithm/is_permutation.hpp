// cmcstl2 - A concept-enabled C++ standard library
//
//  Copyright Eric Niebler 2014
//  Copyright Casey Carter 2015
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/caseycarter/cmcstl2
//
#ifndef STL2_DETAIL_ALGORITHM_IS_PERMUTATION_HPP
#define STL2_DETAIL_ALGORITHM_IS_PERMUTATION_HPP

#include <stl2/functional.hpp>
#include <stl2/iterator.hpp>
#include <stl2/detail/fwd.hpp>
#include <stl2/detail/concepts/algorithm.hpp>

///////////////////////////////////////////////////////////////////////////
// is_permutation [alg.is_permutation]
//
STL2_OPEN_NAMESPACE {
  template <ForwardIterator I1, Sentinel<I1> S1,
            ForwardIterator I2, Sentinel<I2> S2,
            class Pred, class Proj1, class Proj2>
    requires models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  bool __is_permutation_tail(I1 first1, S1 last1, I2 first2, S2 last2,
                             Pred& pred, Proj1& proj1, Proj2& proj2) {
    // For each element in [f1, l1) see if there are the same number of
    //    equal elements in [f2, l2)
    for (I1 i = first1; i != last1; ++i) {
      // Have we already counted the number of *i in [f1, l1)?
      for (I1 j = first1; j != i; ++j) {
        if (pred(proj1(*j), proj1(*i))) {
            goto next_iter;
        }
      }
      {
        // Count number of *i in [f2, l2)
        DifferenceType<I2> c2 = 0;
        for (I2 j = first2; j != last2; ++j) {
          if (pred(proj1(*i), proj2(*j))) {
            ++c2;
          }
        }
        if (c2 == 0) {
          return false;
        }
        // Count number of *i in [i, l1) (we can start with 1)
        DifferenceType<I1> c1 = 1;
        for (I1 j = __stl2::next(i); j != last1; ++j) {
          if (pred(proj1(*i), proj1(*j))) {
            ++c1;
          }
        }
        if (c1 != c2) {
          return false;
        }
      }
    next_iter:;
    }
    return true;
  }

  template<ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2,
           class Pred = equal_to<>, class Proj1 = identity, class Proj2 = identity>
    requires models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  bool is_permutation(I1 first1, S1 last1, I2 first2,
                      Pred pred_ = Pred{},
                      Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    // shorten sequences as much as possible by lopping of any equal parts
    for (; first1 != last1; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        goto not_done;
      }
    }
    return true;
  not_done:

    // first1 != last1 && *first1 != *first2
    auto l1 = __stl2::distance(first1, last1);
    if (l1 == 1) {
      return false;
    }

    return __stl2::__is_permutation_tail(first1, last1, first2, __stl2::next(first2, l1),
                                         pred, proj1, proj2);
  }

  template<ForwardRange Rng1, class I2, ForwardIterator = __uncvref<I2>,
           class Pred = equal_to<>, class Proj1 = identity, class Proj2 = identity>
    requires models::IndirectlyComparable<IteratorType<Rng1>, I2, Pred, Proj1, Proj2>
  bool is_permutation(Rng1&& rng1, I2&& first2, Pred&& pred = Pred{},
                      Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::is_permutation(
      __stl2::begin(rng1), __stl2::end(rng1), first2,
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  template<ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2,
           Sentinel<I2> S2, class Pred = equal_to<>, class Proj1 = identity,
           class Proj2 = identity>
    requires models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  bool is_permutation(I1 first1, S1 last1, I2 first2, S2 last2,
                      Pred pred_ = Pred{},
                      Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);

    // shorten sequences as much as possible by lopping of any equal parts
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        goto not_done;
      }
    }
    return first1 == last1 && first2 == last2;
  not_done:

    // first1 != last1 && first2 != last2 && *first1 != *first2
    auto l1 = __stl2::distance(first1, last1);
    auto l2 = __stl2::distance(first2, last2);
    if (l1 != l2) {
      return false;
    }

    return __stl2::__is_permutation_tail(first1, last1, first2, last2,
                                         pred, proj1, proj2);
  }

  template<ForwardIterator I1, Sentinel<I1> S1, ForwardIterator I2,
           Sentinel<I2> S2, class Pred = equal_to<>, class Proj1 = identity,
           class Proj2 = identity>
    requires SizedIteratorRange<I1, S1>() && SizedIteratorRange<I2, S2>() &&
      models::IndirectlyComparable<I1, I2, Pred, Proj1, Proj2>
  bool is_permutation(I1 first1, S1 last1, I2 first2, S2 last2,
                      Pred pred_ = Pred{},
                      Proj1 proj1_ = Proj1{}, Proj2 proj2_ = Proj2{}) {
    if (__stl2::distance(first1, last1) != __stl2::distance(first2, last2)) {
      return false;
    }

    auto&& pred = __stl2::as_function(pred_);
    auto&& proj1 = __stl2::as_function(proj1_);
    auto&& proj2 = __stl2::as_function(proj2_);
    // shorten sequences as much as possible by lopping of any equal parts
    for (; first1 != last1; ++first1, ++first2) {
      if (!pred(proj1(*first1), proj2(*first2))) {
        goto not_done;
      }
    }
    return true;
  not_done:

    return __stl2::__is_permutation_tail(first1, last1, first2, last2,
                                         pred, proj1, proj2);
  }

  template<ForwardRange Rng1, ForwardRange Rng2, class Pred = equal_to<>,
           class Proj1 = identity, class Proj2 = identity>
    requires models::IndirectlyComparable<IteratorType<Rng1>, IteratorType<Rng2>, Pred, Proj1, Proj2>
  bool is_permutation(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
                      Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    return __stl2::is_permutation(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::end(rng2),
      __stl2::forward<Pred>(pred), __stl2::forward<Proj1>(proj1),
      __stl2::forward<Proj2>(proj2));
  }

  template<ForwardRange Rng1, ForwardRange Rng2, class Pred = equal_to<>,
           class Proj1 = identity, class Proj2 = identity>
    requires SizedRange<Rng1>() && SizedRange<Rng2>() &&
      models::IndirectlyComparable<IteratorType<Rng1>, IteratorType<Rng2>, Pred, Proj1, Proj2>
  bool is_permutation(Rng1&& rng1, Rng2&& rng2, Pred&& pred = Pred{},
                      Proj1&& proj1 = Proj1{}, Proj2&& proj2 = Proj2{}) {
    if (__stl2::size(rng1) != __stl2::size(rng2)) {
      return false;
    }
    return __stl2::is_permutation(
      __stl2::begin(rng1), __stl2::end(rng1),
      __stl2::begin(rng2), __stl2::forward<Pred>(pred),
      __stl2::forward<Proj1>(proj1), __stl2::forward<Proj2>(proj2));
  }
} STL2_CLOSE_NAMESPACE

#endif