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
//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
#ifndef STL2_DETAIL_ALGORITHM_POP_HEAP_HPP
#define STL2_DETAIL_ALGORITHM_POP_HEAP_HPP

#include <stl2/detail/algorithm/heap_sift.hpp>
#include <stl2/detail/range/dangling.hpp>
#include <stl2/detail/range/primitives.hpp>

///////////////////////////////////////////////////////////////////////////
// pop_heap [pop.heap]
//
STL2_OPEN_NAMESPACE {
	namespace detail {
		struct __pop_heap_n_fn {
			template<RandomAccessIterator I, class Proj, class Comp>
			requires Sortable<I, Comp, Proj>
			constexpr void
			operator()(I first, iter_difference_t<I> n, Comp comp, Proj proj) const {
				if (n > 1) {
					iter_swap(first, first + (n - 1));
					sift_down_n(first, n - 1, first, __stl2::ref(comp),
						__stl2::ref(proj));
				}
			}
		};

		inline constexpr __pop_heap_n_fn pop_heap_n {};
	}

	struct __pop_heap_fn : private __niebloid {
		template<RandomAccessIterator I, Sentinel<I> S, class Comp = less,
			class Proj = identity>
		requires Sortable<I, Comp, Proj>
		constexpr I
		operator()(I first, S last, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(first, std::move(last));
			detail::pop_heap_n(first, n, __stl2::ref(comp), __stl2::ref(proj));
			return first + n;
		}

		template<RandomAccessRange Rng, class Comp = less, class Proj = identity>
		requires Sortable<iterator_t<Rng>, Comp, Proj>
		constexpr safe_iterator_t<Rng>
		operator()(Rng&& rng, Comp comp = {}, Proj proj = {}) const {
			auto n = distance(rng);
			detail::pop_heap_n(begin(rng), n, __stl2::ref(comp), __stl2::ref(proj));
			return begin(rng) + n;
		}
	};

	inline constexpr __pop_heap_fn pop_heap {};
} STL2_CLOSE_NAMESPACE

#endif
