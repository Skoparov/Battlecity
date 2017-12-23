#ifndef ENTITY_H
#define ENTITY_H

#include <type_traits>
#include <unordered_map>

#include "id_engine.h"
#include "details/polymorph.h"
#include "../details/spinlock.h"

namespace ecs
{

namespace _detail
{

template< size_t... >struct integer_sequence{};

template< size_t n, size_t... tail >
struct generate_integer_sequence : generate_integer_sequence< n-1, n-1, tail...>{};

template< size_t... seq >
struct generate_integer_sequence< 0, seq... >
{
    using type = integer_sequence< seq... >;
};

}// _detail

using entity_id = numeric_id;
class world;

class entity final
        #ifdef ECS_CONCURRENCY
        : public lockable
        #endif
{
    friend class world;
    using component_id = type_id;
    using component_wrapper = polymorph;

public:
    entity() = default;
    entity( const entity& ) = delete;
    entity( entity&& ) = delete;
    entity& operator=( const entity& ) = delete;
    entity& operator=( entity&& ) = delete;

    template< typename component_type, typename... constructor_args >
    void add_component( constructor_args&&... args )
    {
        component_type component{ std::forward< constructor_args >( args )... };
        component_id id{ get_type_id< component_type >() };

        auto result = m_components.emplace( id, std::move( component ) );
        add_component_to_world( id, result.first->second );
    }

    template< typename component_type >
    bool has_component() const
    {
        return ( m_components.count( get_type_id< component_type >() ) != 0 );
    }

    template< typename component >
    bool has_components() const
    {
        return has_component< component >();
    }

    template< typename component, typename other_component, typename... tail >
    bool has_components() const
    {
        return has_component< component >()?
               has_components< other_component, tail... >() : false;
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

    template< typename component >
    std::tuple< component& > get_components()
    {
        return std::tie( get_component< component >() );
    }

    template< typename component, typename other_component,  typename... tail >
    std::tuple< component&, other_component&, tail&... >
    get_components()
    {
       return std::tuple_cat( std::tie( get_component< component >() ),
                              get_components< other_component, tail... >() );
    }

    template< typename component >
    std::tuple< const component& > get_components() const
    {
            return std::tie( get_component< component >() );
    }

    template< typename component, typename other_component,  typename... tail >
    std::tuple< const component&, const other_component&, const tail&... >
    get_components() const
    {
            return std::tuple_cat( std::tie( get_component< component >() ),
                                   get_components< other_component, tail... >() );
    }

    template< typename component >
    std::tuple< component& > get_components_unsafe() noexcept
    {
        return std::tie( get_component_unsafe< component >() );
    }

    template< typename component, typename other_component,  typename... tail >
    std::tuple< component&, other_component&, tail&... > get_components_unsafe() noexcept
    {
        return std::tuple_cat( std::tie( get_component_unsafe< component >() ),
                               get_components_unsafe< other_component, tail... >() );
    }

    template< typename component >
    std::tuple< const component& > get_components_unsafe() const noexcept
    {
        return std::tie( get_component< component >() );
    }

    template< typename component, typename other_component,  typename... tail >
    std::tuple< const component&, const other_component&, const tail&... >
    get_components_unsafe() const noexcept
    {
        return std::tuple_cat( std::tie( get_component_unsafe< component >() ),
                               get_components_unsafe< other_component, tail... >() );
    }

    template< typename component_type >
    void remove_component()
    {
        component_id id{ get_type_id< component_type >() };
        m_components.erase( id );
        remove_component_from_world( id );
    }

    template< typename component >
    void remove_components()
    {
        remove_component< component >();
    }

    template< typename component, typename other_component,  typename... tail >
    void remove_components()
    {
        remove_component< component >();
        remove_components< other_component, tail... >();
    }

    template< typename... components, typename func_type >
    bool apply_to( func_type&& func )
    {
        return dispatch( get_components< components... >(),
                  std::forward< func_type >( func ),
                  typename _detail::generate_integer_sequence< sizeof...( components ) >::type{} );
    }

    entity_id get_id() const noexcept;
    world& get_world() noexcept;

private:
    entity( world* world, entity_id id ) noexcept;
    void add_component_to_world( const component_id& id, component_wrapper& w );
    void remove_component_from_world( const component_id& id );

    template< typename components_tuple, typename func_type, size_t... seq >
    bool dispatch( components_tuple& tuple,
                   func_type&& func,
                   const _detail::integer_sequence< seq... >& )
    {
        return func( *this, std::get< seq >( tuple )... );
    }

private:
    world* m_world{ nullptr };
    const entity_id m_id{ INVALID_NUMERIC_ID };
    std::unordered_map< component_id, component_wrapper > m_components;
};

bool operator==( const entity& l, const entity& r ) noexcept;
bool operator!=( const entity& l, const entity& r ) noexcept;

}// ecs

#endif
