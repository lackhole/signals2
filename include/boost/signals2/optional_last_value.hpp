// optional_last_value function object (documented as part of Boost.Signals2)

// Copyright Frank Mori Hess 2007-2008.
// Copyright Douglas Gregor 2001-2003.
// Distributed under the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/signals2 for library home page.

#ifndef BOOST_SIGNALS2_OPTIONAL_LAST_VALUE_HPP
#define BOOST_SIGNALS2_OPTIONAL_LAST_VALUE_HPP

#include <optional>
#include <boost/signals2/expired_slot.hpp>

namespace boost {
  namespace signals2 {

    template<typename T>
      class optional_last_value
    {
    public:
      typedef std::optional<T> result_type;

      template<typename InputIterator>
      std::optional<T> operator()(InputIterator first, InputIterator last) const
      {
        std::optional<T> value;
        while (first != last)
        {
          try
          {
            if constexpr (std::is_lvalue_reference_v<T>)
              value = *first;
            else
              value = std::move(*first);
          }
          catch (const expired_slot &) {}
          ++first;
        }
        return value;
      }
    };

    template<>
      class optional_last_value<void>
    {
    public:
      typedef void result_type;
      template<typename InputIterator>
        result_type operator()(InputIterator first, InputIterator last) const
      {
        while (first != last)
        {
          try
          {
            *first;
          }
          catch (const expired_slot &) {}
          ++first;
        }
        return;
      }
    };
  } // namespace signals2
} // namespace boost
#endif // BOOST_SIGNALS2_OPTIONAL_LAST_VALUE_HPP
