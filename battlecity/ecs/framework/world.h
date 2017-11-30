#ifndef WORLD_H
#define WORLD_H

#include <list>
#include <typeinfo>
#include <typeindex>
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

protected:
    world& m_world;
};

//

class world final
{
    using event_id = type_id;
    using event_wrapper = helpers::polymorph;

public:
    world() = default;

    void tick();

    template< typename component_type, typename func_type >
    void for_each( func_type&& func )
    {
        for( auto& entity : m_entities )
        {
            if( entity.second.has_component< component_type >() )
            {
                entity.second.apply_to< component_type >( std::forward< func_type >( func ) );
            }
        }
    }

    entity& create_entity();
    void schedule_destroy_entity( entity& e );

    void add_system( system& system );
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
                event_callback< event_type >* callback{ reinterpret_cast< event_callback< event_type >* >( subscriber ) };
                callback->on_event( event );
            }
        }
    }

private:
    void cleanup();

private:
    std::unordered_set< system* > m_systems;
    std::unordered_map< entity_id, entity > m_entities;

    std::list< system* > m_systems_to_remove;
    std::list< entity_id > m_entities_to_remove;

    std::unordered_map< event_id, std::unordered_set< detail::event_callback_base* > > m_subscribers;
};

}// ecs

#endif
