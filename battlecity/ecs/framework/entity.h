#ifndef ENTITY_H
#define ENTITY_H

#include <unordered_map>

#include "id_engine.h"
#include "details/polymorph.h"

namespace ecs
{

using entity_id = numeric_id;

class entity final
{
    using component_id = type_id;
    using component_wrapper = helpers::polymorph;

    friend class world;

public:
    ~entity() = default;

    template< typename component_type, typename... constructor_args >
    void add_component( constructor_args&&... args )
    {
        component_type component{ std::forward< constructor_args >( args )... };
        m_components[ get_type_id< component_type >() ] = std::move( component );
    }

    template< typename component_type >
    void remove_component()
    {
        m_components.erase( get_type_id< component_type >() );
    }

    template< typename component_type >
    component_type& get_component_unsafe()
    {
        component_wrapper& ch = m_components.at( get_type_id< component_type >() );
        return ch.get_unsafe< component_type >();
    }

    template< typename component_type >
    const component_type& get_component_unsafe() const
    {
        const component_wrapper& ch = m_components.at( get_type_id< component_type >() );
        return ch.get_unsafe< component_type >();
    }

    template< typename component_type >
    component_type& get_component()
    {
        component_wrapper& ch = m_components.at( get_type_id< component_type >() );
        return ch.get< component_type >();
    }

    template< typename component_type >
    const component_type& get_component() const
    {
        const component_wrapper& ch = m_components.at( get_type_id< component_type >() );
        return ch.get< component_type >();
    }

    bool has_component( const component_id& get_id ) const;

    template< typename component_type >
    bool has_component() const
    {
        return m_components.count( get_type_id< component_type >() ) != 0;
    }

    entity_id get_id() const noexcept;

private:
    entity() = default;
    explicit entity( entity_id get_id ) noexcept;

    template< typename component_type, typename function_type >
    bool apply_to( function_type&& func )
    {
        bool need_to_continue{ true };

        auto it = m_components.find( get_type_id< component_type >() );
        if( it != m_components.end() )
        {
            component_wrapper& ch = it->second;
            need_to_continue = func( *this, ch.get< component_type >() );
        }

        return need_to_continue;
    }

private:
    const entity_id m_id{ INVALID_NUMERIC_ID };
    std::unordered_map< component_id, component_wrapper > m_components;
};

bool operator==( const entity& l, const entity& r ) noexcept;
bool operator!=( const entity& l, const entity& r ) noexcept;

}// ecs

#endif
