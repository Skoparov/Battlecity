#ifndef INTEGER_SEQUENCE_H
#define INTEGER_SEQUENCE_H

#include <cstddef>
#include <type_traits>

namespace ecs
{

template< typename type, type... values >
struct integer_sequence{};

namespace _detail
{

template< typename type, size_t size, type... ints >
struct make_integer_sequence_impl : make_integer_sequence_impl< type, size - 1, size - 1, ints... >{};

template< typename type, type... ints >
struct make_integer_sequence_impl< type, 0u, ints... > : integer_sequence< type, ints... >
{ using integer_sequence_type = integer_sequence< type, ints... >; };

}//_detail

template< typename type, size_t size >
using make_integer_sequence = typename _detail::make_integer_sequence_impl< type, size >::integer_sequence_type;

template< size_t size >
using make_index_sequence = make_integer_sequence< size_t, size >;

template< typename... args >
using index_sequence_for = make_index_sequence< sizeof...( args ) >;

}// std

#endif
