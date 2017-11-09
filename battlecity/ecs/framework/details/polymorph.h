#ifndef _HELPERS_POLYMORPH_H_
#define _HELPERS_POLYMORPH_H_

#include <memory>
#include <typeinfo>

namespace helpers
{

namespace _details{ class base_type_storage; }

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

    polymorph( const polymorph& other ) noexcept;
    polymorph( polymorph&& other ) noexcept;
    polymorph& operator=( const polymorph& other ) noexcept;
    polymorph& operator=( polymorph&& other ) noexcept;

    // Creates a deep copy of source's data instead of sharing pointer copy
    template< class T >
    polymorph& deep_copy( const polymorph& source );

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
    std::shared_ptr< _details::base_type_storage > m_data;
};

}// helpers

#include "polymorph.impl"

#endif
