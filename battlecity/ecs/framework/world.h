#ifndef WORLD_H
#define WORLD_H

#include <list>
#include <algorithm>
#include <type_traits>
#include <unordered_set>

#include "entity.h"

namespace ecs
{

class world;

namespace detail
{

class event_callback_base{};

}// details

template< typename event_type >
class event_callback : public detail::event_callback_base
{
public:
    virtual void on_event( const event_type& event ) = 0;
    virtual ~event_callback() = default;
};

//

class system
{
public:
    system( world& world ) noexcept;
    virtual ~system() = default;
    virtual void tick() = 0;
    virtual void clean(){}

protected:
    world& m_world;
};

//

class world final
{
    using event_id = type_id;
    using component_info = std::pair< entity::component_wrapper*, entity* >;

    friend class entity;

public:
    world() = default;

    void tick(); // tick() of each system

    void reset(); // remove all entities, clean() systems
    void clean(); // remove all entities and systems

    entity& create_entity();
    void remove_entity( entity& e );
    void schedule_remove_entity( entity& e );

    template< typename component_type >
    std::list< component_type* > get_components()
    {
        std::list< component_type* > components;

        auto eq_range = m_components.equal_range( get_type_id< component_type >() );
        for( auto it = eq_range.first; it != eq_range.second; ++it )
        {
            entity::component_wrapper* w{ it->second.first };
            components.emplace_back( &w->get< component_type >() );
        }

        return components;
    }

    template< typename component_type >
    std::list< entity* > get_entities_with_component()
    {
        std::list< entity* > entities;

        auto eq_range = m_components.equal_range( get_type_id< component_type >() );
        for( auto it = eq_range.first; it != eq_range.second; ++it )
        {
            entities.emplace_back( it->second.second );
        }

        return entities;
    }

    // func should be of signature bool< entity&, component_type& >
    // where bool indicates whether for_each loop should continue execution upon function return
    template< typename component_type, typename func_type >
    void for_each( func_type&& func )
    {
        auto eq_range = m_components.equal_range( get_type_id< component_type >() );
        for( auto it = eq_range.first; it != eq_range.second; ++it )
        {
            entity::component_wrapper* w{ it->second.first };
            entity& e = *it->second.second;

            if( !func( e, w->get< component_type >() ) )
            {
                break;
            }
        }
    }

    void add_system( system& system );
    void remove_system( system& s );
    void schedule_remove_system( system& system );

    template< typename event_type >
    void subscribe( event_callback< event_type >& callback )
    {
        m_subscribers[ get_type_id< event_type >() ].emplace( &callback );
    }

    template< typename event_type >
    void unsubscribe( event_callback< event_type >& callback )
    {
        m_subscribers.at( get_type_id< event_type > ).erase( &callback );
    }

    template< typename event_type >
    void emit_event( const event_type& event )
    {
        auto it = m_subscribers.find( get_type_id< event_type >() );
        if( it != m_subscribers.end() )
        {
            for( auto& subscriber : it->second )
            {
                event_callback< event_type >* callback{ static_cast< event_callback< event_type >* >( subscriber ) };
                callback->on_event( event );
            }
        }
    }

private:
    void add_component( entity& e, const entity::component_id& id, entity::component_wrapper& w );
    void remove_component( entity& e, const entity::component_id& c_id );
    void cleanup();

private:
    std::unordered_set< system* > m_systems;
    std::unordered_map< entity_id, entity > m_entities;
    std::unordered_multimap< entity::component_id, component_info > m_components;

    std::list< system* > m_systems_to_remove;
    std::list< entity_id > m_entities_to_remove;

    std::unordered_map< event_id, std::unordered_set< detail::event_callback_base* > > m_subscribers;
};

}// ecs

#endif
