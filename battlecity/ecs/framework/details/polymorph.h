#ifndef ECS_POLYMORPH_H
#define ECS_POLYMORPH_H

#include <memory>
#include <typeinfo>

namespace ecs
{

namespace _detail{ class base_type_storage; }

class polymorph
{
  template< typename type >
  using disable_if_polymorph = typename std::enable_if< !std::is_same<
    typename std::decay< type >::type, polymorph >::value >::type;

public:
    polymorph() = default;

    template< typename type, typename = disable_if_polymorph< type > >
    polymorph( type&& t ); // internal data assignment  constructor

    template< typename type, typename = disable_if_polymorph< type > >
    polymorph& operator=( type&& t ); // internal data assignment operator

    polymorph( const polymorph& other ) = delete;
    polymorph( polymorph&& other ) noexcept;
    polymorph& operator=( const polymorph& other ) = delete;
    polymorph& operator=( polymorph&& other ) noexcept;

    template< typename type >
    type& get();

    template< typename type >
    const type& get() const;

    // unsafe get functions use static_cast, no runtime checks are performed
    template< typename type >
    type& get_unsafe() noexcept;

    template< typename type >
    const type& get_unsafe() const noexcept;

    template< typename type >
    bool check_type() const noexcept;

    const std::type_info& type_info() const noexcept;

    bool empty() const noexcept;

private:
    std::unique_ptr< _detail::base_type_storage > m_data;
};

}// ecs

#include "polymorph.impl"

#endif
