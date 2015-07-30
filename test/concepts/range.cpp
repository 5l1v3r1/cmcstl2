#include "validate.hpp"

#if VALIDATE_RANGES
#include <range/v3/utility/concepts.hpp>
#include <range/v3/range_concepts.hpp>
#include <range/v3/range_traits.hpp>

#define NAMESPACE ranges

namespace ns {
template <class R>
using IteratorType = ranges::range_iterator_t<R>;

template <class R>
using SentinelType = ranges::range_sentinel_t<R>;

using ranges::size;
}

namespace models {
template <class T, class U>
using same = ranges::Same<T, U>;

template <class R>
using range = ranges::Range<R>;

template <class R>
using sized_range = ranges::SizedRange<R>;

template <class R>
using sized_range_like = ranges::SizedRangeLike_<R>;
}

template <bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
  CONCEPT_REQUIRES(allow_nonconst)
  int* begin();
  CONCEPT_REQUIRES(allow_nonconst)
  int* end();
  CONCEPT_REQUIRES(allow_const)
  const int* begin() const;
  CONCEPT_REQUIRES(allow_const)
  const int* end() const;

  CONCEPT_REQUIRES(allow_size)
  int size() const;
};

#elif VALIDATE_STL2
#include <stl2/detail/concepts/range.hpp>

#define NAMESPACE stl2

namespace ns {
using stl2::DifferenceType;
using stl2::IteratorType;
using stl2::SentinelType;

using stl2::size;
}

template <bool allow_nonconst, bool allow_const, bool allow_size>
struct arbitrary_range {
  int* begin() requires allow_nonconst;
  int* end() requires allow_nonconst;
  const int* begin() const requires allow_const;
  const int* end() const requires allow_const;

  int size() const requires allow_size;
};
#endif

#include "../simple_test.hpp"

using mutable_unsized_range =
  arbitrary_range<true, true, false>;

using mutable_only_unsized_range =
  arbitrary_range<true, false, false>;

using immutable_unsized_range =
  arbitrary_range<false, true, false>;

using mutable_sized_range =
  arbitrary_range<true, true, true>;

using mutable_only_sized_range =
  arbitrary_range<true, false, true>;

using immutable_sized_range =
  arbitrary_range<false, true, true>;

// size() launches the missiles.
struct mutable_badsized_range :
  mutable_sized_range {};

struct mutable_only_badsized_range :
  mutable_only_sized_range {};

struct immutable_badsized_range :
  immutable_sized_range {};

namespace NAMESPACE {
template <>
struct is_sized_range<mutable_badsized_range> :
  std::false_type {};
template <>
struct is_sized_range<mutable_only_badsized_range> :
  std::false_type {};
template <>
struct is_sized_range<immutable_badsized_range> :
  std::false_type {};
}

int main() {
  {
    CHECK(!models::range<void>());
    CHECK(!models::sized_range_like<void>());
    CHECK(!models::sized_range<void>());
  }

  using I = int*;
  using CI = const int*;

  {
    using T = int[2];

    {
      CHECK(!models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(models::sized_range_like<const T>());
      CHECK(models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(models::sized_range_like<const T&>());
      CHECK(models::sized_range<const T&>());
    }
  }

  {
    using T = mutable_unsized_range;
    {
      CHECK(models::same<ns::IteratorType<T>, CI>());
      CHECK(models::same<ns::SentinelType<T>, CI>());
      CHECK(models::range<T>());
      CHECK(!models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(!models::sized_range_like<T&>());
      CHECK(!models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(!models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(!models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  {
    using T = mutable_only_unsized_range;
    {
      CHECK(!models::range<T>());
      CHECK(!models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(!models::sized_range_like<T&>());
      CHECK(!models::sized_range<T&>());
    }
    {
      CHECK(!models::range<const T>());
      CHECK(!models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(!models::range<const T&>());
      CHECK(!models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  {
    using T = immutable_unsized_range;
    {
      CHECK(models::same<ns::IteratorType<T>, CI>());
      CHECK(models::same<ns::SentinelType<T>, CI>());
      CHECK(models::range<T>());
      CHECK(!models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, CI>());
      CHECK(models::same<ns::SentinelType<T&>, CI>());
      CHECK(models::range<T&>());
      CHECK(!models::sized_range_like<T&>());
      CHECK(!models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(!models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(!models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  {
    using T = mutable_sized_range;
    {
      CHECK(models::same<ns::IteratorType<T>, CI>());
      CHECK(models::same<ns::SentinelType<T>, CI>());
      CHECK(models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(models::sized_range_like<const T>());
      CHECK(models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(models::sized_range_like<const T&>());
      CHECK(models::sized_range<const T&>());
    }
  }

  {
    using T = mutable_only_sized_range;
    {
      CHECK(!models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(models::sized_range<T&>());
    }
    {
      CHECK(!models::range<const T>());
      CHECK(!models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(!models::range<const T&>());
      CHECK(!models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  {
    using T = immutable_sized_range;
    {
      CHECK(models::same<ns::IteratorType<T>, CI>());
      CHECK(models::same<ns::SentinelType<T>, CI>());
      CHECK(models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, CI>());
      CHECK(models::same<ns::SentinelType<T&>, CI>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(models::sized_range_like<const T>());
      CHECK(models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(models::sized_range_like<const T&>());
      CHECK(models::sized_range<const T&>());
    }
  }

  {
    using T = mutable_badsized_range;
    {
      CHECK(models::same<ns::IteratorType<T>, CI>());
      CHECK(models::same<ns::SentinelType<T>, CI>());
      CHECK(models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(!models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  {
    using T = mutable_only_badsized_range;
    {
      CHECK(!models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, I>());
      CHECK(models::same<ns::SentinelType<T&>, I>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(!models::sized_range<T&>());
    }
    {
      CHECK(!models::range<const T>());
      CHECK(!models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(!models::range<const T&>());
      CHECK(!models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  {
    using T = immutable_badsized_range;
    {
      CHECK(models::same<ns::IteratorType<T>, CI>());
      CHECK(models::same<ns::SentinelType<T>, CI>());
      CHECK(models::range<T>());
      CHECK(models::sized_range_like<T>());
      CHECK(!models::sized_range<T>());
    }
    {
      CHECK(models::same<ns::IteratorType<T&>, CI>());
      CHECK(models::same<ns::SentinelType<T&>, CI>());
      CHECK(models::range<T&>());
      CHECK(models::sized_range_like<T&>());
      CHECK(!models::sized_range<T&>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T>, CI>());
      CHECK(models::same<ns::SentinelType<const T>, CI>());
      CHECK(models::range<const T>());
      CHECK(models::sized_range_like<const T>());
      CHECK(!models::sized_range<const T>());
    }
    {
      CHECK(models::same<ns::IteratorType<const T&>, CI>());
      CHECK(models::same<ns::SentinelType<const T&>, CI>());
      CHECK(models::range<const T&>());
      CHECK(models::sized_range_like<const T&>());
      CHECK(!models::sized_range<const T&>());
    }
  }

  return ::test_result();
}
