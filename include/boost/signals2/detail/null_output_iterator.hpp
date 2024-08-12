/*
  An output iterator which simply discards output.
*/
// Copyright Frank Mori Hess 2008.
// Distributed under the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/signals2 for library home page.

#ifndef BOOST_SIGNALS2_NULL_OUTPUT_ITERATOR_HPP
#define BOOST_SIGNALS2_NULL_OUTPUT_ITERATOR_HPP

#include <type_traits>

namespace boost
{
  namespace signals2
  {
    namespace detail
    {
      template <class UnaryFunction>
      class function_output_iterator {
       private:
        typedef function_output_iterator self;

        class output_proxy {
         public:
          explicit output_proxy(UnaryFunction& f) noexcept : m_f(f) { }

          template <class T>
          typename std::enable_if_t<
            !std::is_same< std::remove_cv_t< std::remove_reference_t< T > >, output_proxy >::value,
            output_proxy&
          > operator=(T&& value) {
            m_f(static_cast< T&& >(value));
            return *this;
          }

          output_proxy(output_proxy const& that) = default;
          output_proxy& operator=(output_proxy const&) = delete;

         private:
          UnaryFunction& m_f;
        };

       public:
        typedef std::output_iterator_tag iterator_category;
        typedef void                value_type;
        typedef void                difference_type;
        typedef void                pointer;
        typedef void                reference;

        explicit function_output_iterator() {}

        explicit function_output_iterator(const UnaryFunction& f)
          : m_f(f) {}

        output_proxy operator*() { return output_proxy(m_f); }
        self& operator++() { return *this; }
        self& operator++(int) { return *this; }

       private:
        UnaryFunction m_f;
      };

      class does_nothing
      {
      public:
        template<typename T>
          void operator()(const T&) const
          {}
      };
      typedef function_output_iterator<does_nothing> null_output_iterator;
    } // namespace detail
  } // namespace signals2
} // namespace boost

#endif  // BOOST_SIGNALS2_NULL_OUTPUT_ITERATOR_HPP
