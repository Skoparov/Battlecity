#ifndef ID_ENGINE_H
#define ID_ENGINE_H

#include <typeinfo>
#include <typeindex>
#include <cstdint>

namespace ecs
{

using type_id = std::type_index;
using numeric_id = uint32_t;

#define INVALID_NUMERIC_ID 0

template< typename type >
constexpr type_id get_type_id() noexcept{ return { typeid( type ) }; }

numeric_id generate_numeric_id();

}// ecs

#endif
