// Boost.Signals2 library

// Copyright Frank Mori Hess 2007-2008.
// Copyright Timmo Stange 2007.
// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#ifndef BOOST_SIGNALS2_TRACKED_OBJECTS_VISITOR_HPP
#define BOOST_SIGNALS2_TRACKED_OBJECTS_VISITOR_HPP

#include <functional>
#include <memory>
#include <type_traits>

#include <boost/signals2/detail/signals_common.hpp>
#include <boost/signals2/slot_base.hpp>
#include <boost/signals2/trackable.hpp>


namespace boost
{
  namespace signals2
  {
    namespace detail
    {
      // Visitor to collect tracked objects from a bound function.
      class tracked_objects_visitor
      {
      public:
        tracked_objects_visitor(slot_base *slot) : slot_(slot)
        {}
        template<typename T>
        void operator()(const T& t) const
        {
            m_visit_reference_wrapper(t, is_reference_wrapper<T>{});
        }
      private:
        template<typename T>
        void m_visit_reference_wrapper(const std::reference_wrapper<T> &t, const std::true_type &) const
        {
            m_visit_pointer(t.get_pointer(), std::true_type{});
        }
        template<typename T>
        void m_visit_reference_wrapper(const T &t, const std::false_type &) const
        {
            m_visit_pointer(t, std::is_pointer<T>{});
        }
        template<typename T>
        void m_visit_pointer(const T &t, const std::true_type &) const
        {
            m_visit_not_function_pointer(t, std::negation<std::is_function<std::remove_pointer_t<T>>>{});
        }
        template<typename T>
        void m_visit_pointer(const T &t, const std::false_type &) const
        {
            m_visit_pointer(std::addressof(t), std::true_type{});
        }
        template<typename T>
        void m_visit_not_function_pointer(const T *t, const std::true_type &) const
        {
            m_visit_signal(t, is_signal<T>{});
        }
        template<typename T>
        void m_visit_not_function_pointer(const T &, const std::false_type &) const
        {}
        template<typename T>
        void m_visit_signal(const T *signal, const std::true_type &) const
        {
          if(signal)
            slot_->track_signal(*signal);
        }
        template<typename T>
        void m_visit_signal(const T &t, const std::false_type &) const
        {
            add_if_trackable(t);
        }
        void add_if_trackable(const trackable *trackable) const
        {
          if(trackable)
            slot_->_tracked_objects.push_back(trackable->get_weak_ptr());
        }
        void add_if_trackable(const void *) const {}

        mutable slot_base * slot_;
      };


    } // end namespace detail
  } // end namespace signals2
} // end namespace boost

#endif // BOOST_SIGNALS2_TRACKED_OBJECTS_VISITOR_HPP

