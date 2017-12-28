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

namespace _detail
{

class event_callback_base{};

}// _detail


// All classes subscribing to a certain event should inherit a specialization of this interface
template< typename event_type >
class event_callback : public _detail::event_callback_base
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

    virtual void init(){}
    virtual bool tick() = 0;
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

    // calls tick() of each system, clears objects schduled to be removed
    bool tick();

    void reset(); // remove all entities, clean() systems
    void clean(); // remove all entities and systems

    entity& create_entity();
    entity& get_entity( entity_id id );
    bool entity_present( entity_id id ) const noexcept;

    void remove_entity( entity& e );
    void remove_entity( ecs::entity_id id );
    void schedule_remove_entity( entity& e );
    void schedule_remove_entity( ecs::entity_id id );

    template< typename component_type >
    std::list< entity* > get_entities_with_component()
    {
        std::list< entity* > entities;

        auto it = m_components.find( get_type_id< component_type >() );
        if( it !=  m_components.end() )
        {
            for( auto& value_pair : it->second )
            {
                entities.emplace_back( value_pair.first );
            }
        }

        return entities;
    }

    // func should be of signature bool< entity&, component_type_1&, ..., component_type_n&.... >
    // where bool indicates whether for_each loop should continue execution upon function return
    template< typename component_type, typename... other_components, typename func_type >
    void for_each_with( func_type&& func )
    {
        auto it = m_components.find( get_type_id< component_type >() );
        if( it !=  m_components.end() )
        {
            for( auto value_pair : it->second )
            {
                entity& e = *value_pair.first;

                if( e.get_state() == entity_state::ok &&
                    e.has_components< component_type, other_components... >() &&
                    !e.apply_to< component_type, other_components... >( func ) )
                {
                    break;
                }
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
        auto it = m_subscribers.find( get_type_id< event_type >() );
        if( it != m_subscribers.end() )
        {
            it->second.erase( &callback );
        }
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
    void remove_component( entity& e, const entity::component_id& id );
    void cleanup();

private:
    std::unordered_set< system* > m_systems;
    std::unordered_map< entity_id, std::unique_ptr< entity > > m_entities;
    std::unordered_map< entity::component_id,
                        std::unordered_map< entity*, entity::component_wrapper* > > m_components;

    std::unordered_set< system* > m_systems_to_remove;
    std::unordered_set< entity* > m_entities_to_remove;

    std::unordered_map< event_id, std::unordered_set< _detail::event_callback_base* > > m_subscribers;
};

}// ecs

#endif
