/*
  Helper class used by variadic implementation of variadic boost::signals2::signal.

  Author: Frank Mori Hess <fmhess@users.sourceforge.net>
  Begin: 2009-05-27
*/
// Copyright Frank Mori Hess 2009
// Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS2_DETAIL_VARIADIC_SLOT_INVOKER_HPP
#define BOOST_SIGNALS2_DETAIL_VARIADIC_SLOT_INVOKER_HPP

#include <boost/signals2/detail/variadic_arg_type.hpp>

#include <tuple>

// vc12 seems to erroneously report formal parameters as unreferenced (warning C4100)
// if parameters of variadic template functions are only referenced by calling
// other varadic template functions. silence these warnings:
#if defined(BOOST_MSVC)
#pragma warning(push)
#if  BOOST_MSVC >= 1800
#pragma warning(disable:4100)
#endif
#endif

namespace boost
{
  namespace signals2
  {
    namespace detail
    {
      template<unsigned ... values> class unsigned_meta_array {};

      template<typename UnsignedMetaArray, unsigned n> class unsigned_meta_array_appender;

      template<unsigned n, unsigned ... Args>
        class unsigned_meta_array_appender<unsigned_meta_array<Args...>, n>
      {
      public:
        typedef unsigned_meta_array<Args..., n> type;
      };

      template<unsigned n> class make_unsigned_meta_array;

      template<> class make_unsigned_meta_array<0>
      {
      public:
        typedef unsigned_meta_array<> type;
      };

      template<> class make_unsigned_meta_array<1>
      {
      public:
        typedef unsigned_meta_array<0> type;
      };

      template<unsigned n> class make_unsigned_meta_array
      {
      public:
        typedef typename unsigned_meta_array_appender<typename make_unsigned_meta_array<n-1>::type, n - 1>::type type;
      };

      template<typename R>
        class call_with_tuple_args
      {
      public:
        typedef R result_type;

        template<typename Func, typename ... Args, std::size_t N>
        R operator()(Func &func, const std::tuple<Args...> & args, std::integral_constant<std::size_t, N>) const
        {
          typedef typename make_unsigned_meta_array<N>::type indices_type;
          return m_invoke<Func>(func, indices_type(), args);
        }
      private:
        template<typename Func, unsigned ... indices, typename ... Args>
          R m_invoke(Func &func, unsigned_meta_array<indices...>, const std::tuple<Args...> & args,
            typename std::enable_if<!std::is_void_v<typename Func::result_type> >::type * = 0
          ) const
        {
          return func(std::get<indices>(args)...);
        }
        template<typename Func, unsigned ... indices, typename ... Args>
          R m_invoke(Func &func, unsigned_meta_array<indices...>, const std::tuple<Args...> & args,
            typename std::enable_if<std::is_void_v<typename Func::result_type> >::type * = 0
          ) const
        {
          func(std::get<indices>(args)...);
          return R();
        }
        // This overload is redundant, as it is the same as the previous variadic method when
        // it has zero "indices" or "Args" variadic template parameters.  This overload
        // only exists to quiet some unused parameter warnings
        // on certain compilers (some versions of gcc and msvc)
        template<typename Func>
          R m_invoke(Func &func, unsigned_meta_array<>, const std::tuple<> &,
            typename std::enable_if<std::is_void_v<typename Func::result_type> >::type * = 0
          ) const
        {
          func();
          return R();
        }
      };

      template<typename R, typename ... Args>
        class variadic_slot_invoker
      {
      public:
        typedef R result_type;

        variadic_slot_invoker(Args & ... args): _args(args...)
        {}
        template<typename ConnectionBodyType>
          result_type operator ()(const ConnectionBodyType &connectionBody) const
        {
          return call_with_tuple_args<result_type>()(connectionBody->slot().slot_function(), 
            _args, std::integral_constant<std::size_t, sizeof...(Args)>{});
        }
      private:
        std::tuple<Args& ...> _args;
      };
    } // namespace detail
  } // namespace signals2
} // namespace boost

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif


#endif // BOOST_SIGNALS2_DETAIL_VARIADIC_SLOT_INVOKER_HPP
