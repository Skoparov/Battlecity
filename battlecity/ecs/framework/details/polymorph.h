#ifndef _POLYMORPH_H_
#define _POLYMORPH_H_

#include <memory>
#include <typeinfo>

namespace ecs
{

namespace _detail{ class base_type_storage; }

class polymorph
{
  template< typename T >
  using disable_if_polymorph = typename std::enable_if< !std::is_same<
    typename std::decay< T >::type, polymorph >::value >::type;

public:
    polymorph() = default;

    template< typename T, typename = disable_if_polymorph< T > >
    polymorph( T&& t ); // internal data assignment  constructor

    template< typename T, typename = disable_if_polymorph< T > >
    polymorph& operator=( T&& t ); // internal data assignment operator

    polymorph( const polymorph& other ) = delete;
    polymorph( polymorph&& other ) noexcept;
    polymorph& operator=( const polymorph& other ) = delete;
    polymorph& operator=( polymorph&& other ) noexcept;

    template< class T >
    T& get();

    template< class T >
    const T& get() const;

    // unsafe get functions use static_cast, no runtime checks are performed
    template< class T >
    T& get_unsafe() noexcept;

    template< class T >
    const T& get_unsafe() const noexcept;

    template< typename T >
    bool check_type() const noexcept;

    const std::type_info& type_info() const noexcept;

    bool empty() const noexcept;

private:
    std::unique_ptr< _detail::base_type_storage > m_data;
};

}// ecs

#include "polymorph.impl"

#endif
