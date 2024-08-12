// last_value function object (documented as part of Boost.Signals)

// Copyright Frank Mori Hess 2007.
// Copyright Douglas Gregor 2001-2003. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS2_LAST_VALUE_HPP
#define BOOST_SIGNALS2_LAST_VALUE_HPP

#include <exception>
#include <optional>
#include <stdexcept>
#include <type_traits>

#include <boost/signals2/expired_slot.hpp>

namespace boost {

template<typename T, typename U>
constexpr auto&& forward_like(U&& x) noexcept {
  constexpr bool is_adding_const = std::is_const_v<std::remove_reference_t<T>>;
  if constexpr (std::is_lvalue_reference_v<T&&>)
  {
    if constexpr (is_adding_const)
      return std::as_const(x);
    else
      return static_cast<U&>(x);
  }
  else
  {
    if constexpr (is_adding_const)
      return std::move(std::as_const(x));
    else
      return std::move(x);
  }
}

  namespace signals2 {

    // no_slots_error is thrown when we are unable to generate a return value
    // due to no slots being connected to the signal.
    class no_slots_error: public std::exception
    {
    public:
      virtual const char* what() const throw() {return "boost::signals2::no_slots_error";}
    };

    template<typename T>
    class last_value {
    public:
      typedef T result_type;

      template<typename InputIterator>
      T operator()(InputIterator first, InputIterator last) const
      {
        if(first == last)
        {
          throw(no_slots_error());
        }
        std::optional<T> value;
        while (first != last)
        {
          try
          {
            value = forward_like<T>(*first);
          }
          catch (const expired_slot &) {}
          ++first;
        }
        if(value) return *value;
        throw (no_slots_error());
      }
    };

    template<>
    class last_value<void> {
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
#endif // BOOST_SIGNALS2_LAST_VALUE_HPP
