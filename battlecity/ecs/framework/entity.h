#ifndef ENTITY_H
#define ENTITY_H

#include <type_traits>
#include <unordered_map>

#include "id_engine.h"
#include "details/polymorph.h"

namespace ecs
{

using entity_id = numeric_id;

class entity final
{
    friend class world;
    using component_id = type_id;
    using component_wrapper = polymorph;

public:
    entity() = default;
    entity( const entity& ) = delete;
    entity( entity&& ) = default;
    entity& operator=( const entity& ) = delete;
    entity& operator=( entity&& ) = default;

    template< typename component_type, typename... constructor_args >
    void add_component( constructor_args&&... args )
    {
        component_type component{ std::forward< constructor_args >( args )... };
        component_id id{ get_type_id< component_type >() };

        auto result = m_components.emplace( id, std::move( component ) );
        m_world->add_component( *this, id, result.first->second );
    }

    template< typename component_type >
    void remove_component()
    {
        component_id id{ get_type_id< component_type >() };
        m_components.erase( id );
        m_world.remove_component( *this, id );
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


    template< typename component_type >
    bool has_component() const
    {
        return ( m_components.count( get_type_id< component_type >() ) != 0 );
    }

    entity_id get_id() const noexcept;

private:
    entity( world* world, entity_id id ) noexcept;

private:
    world* m_world{ nullptr };
    const entity_id m_id{ INVALID_NUMERIC_ID };
    std::unordered_map< component_id, component_wrapper > m_components;
};

bool operator==( const entity& l, const entity& r ) noexcept;
bool operator!=( const entity& l, const entity& r ) noexcept;

}// ecs

#endif
