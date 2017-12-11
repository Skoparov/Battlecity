#include "world.h"

namespace ecs
{

entity_id generate_entity_id( const std::unordered_map< entity_id, entity >& present_entries )
{
    numeric_id id{ generate_numeric_id() };
    while( present_entries.count( id ) )
    {
        id = generate_numeric_id();
    }

    return id;
}

void world::tick()
{
    cleanup();

    for( system* system : m_systems )
    {
        system->tick();
    }
}

void world::reset()
{
    m_entities.clear();
    m_components.clear();
    m_entities_to_remove.clear();
    m_systems_to_remove.clear();

    for( system* system : m_systems )
    {
        system->clean();
    }
}

void world::clean()
{
    m_entities.clear();
    m_components.clear();
    m_systems.clear();
    m_entities_to_remove.clear();
    m_systems_to_remove.clear();
}

entity& world::create_entity()
{
    entity_id id{ generate_entity_id( m_entities ) };
    auto res = m_entities.emplace( id, entity{ this, id } );
    return res.first->second;
}

void world::remove_entity( entity& e )
{
    for( const auto& component_info : e.m_components )
    {
        remove_component( e, component_info.first );
    }

    m_entities.erase( e.get_id() );
}

void world::schedule_remove_entity( entity& e )
{
    m_entities_to_remove.emplace_back( e.get_id() );
}

void world::add_system( system& system )
{
    m_systems.emplace( &system );
}

void world::remove_system( system& s )
{
    m_systems.erase( &s );
}

void world::schedule_remove_system( system& system )
{
    m_systems_to_remove.emplace_back( &system );
}

void world::add_component( entity& e, const entity::component_id& id, entity::component_wrapper& w )
{
    component_info info{ &w, &e };
    m_components.emplace( std::make_pair( id, info ) );
}

void world::remove_component( entity& e, const entity::component_id& c_id )
{
    using value_type = std::unordered_multimap< entity::component_id, component_info >::value_type;

    auto eq_range = m_components.equal_range( c_id );
    for( auto it = eq_range.first; it != eq_range.second; )
    {
        const component_info& info = it->second;
        if ( info.second->get_id() == e.get_id() )
        {
            m_components.erase( it++ );
        }
        else
        {
            ++it;
        }
    }
}

void world::cleanup()
{
    for( entity_id id : m_entities_to_remove )
    {
        remove_entity( m_entities[ id ] );
    }

    for( system* s : m_systems_to_remove )
    {
        m_systems.erase( s );
    }

    m_entities_to_remove.clear();
    m_systems_to_remove.clear();
}

system::system( world &world ) noexcept: m_world( world ){}

}// ecs
