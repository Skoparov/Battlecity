#include "world.h"

namespace ecs
{

entity_id generate_entity_id( const std::unordered_map< entity_id, std::unique_ptr< entity > >& present_entries )
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
    std::unique_ptr< entity > e{ new entity{ this, id } };
    auto res = m_entities.emplace( id, std::move( e ) );
    return *res.first->second;
}

bool world::entity_present( entity_id id ) const noexcept
{
    return ( m_entities.count( id ) != 0 );
}

entity& world::get_entity( entity_id id )
{
    return *m_entities.at( id );
}

void world::remove_entity( entity& e )
{
    for( const auto& component_info : e.m_components )
    {
        remove_component( e, component_info.first );
    }

    m_entities.erase( e.get_id() );
}

void world::remove_entity( entity_id id )
{
    entity& e = *m_entities.at( id );
    remove_entity( e );
}

void world::schedule_remove_entity( entity& e )
{
    e.set_state( entity_state::invalid );
    m_entities_to_remove.emplace( &e );
}

void world::schedule_remove_entity( entity_id id )
{
    schedule_remove_entity( *m_entities.at( id ) );
}

void world::add_system( system& system )
{
    system.init();
    m_systems.emplace( &system );
}

void world::remove_system( system& s )
{
    m_systems.erase( &s );
}

void world::schedule_remove_system( system& system )
{
    m_systems_to_remove.emplace( &system );
}

void world::add_component( entity& e, const entity::component_id& id, entity::component_wrapper& w )
{
    m_components[ id ].emplace( &e, &w );
}

void world::remove_component( entity& e, const entity::component_id& id )
{
    auto it = m_components.find( id );
    if( it !=  m_components.end() )
    {
        it->second.erase( &e );
        if( it->second.empty() )
        {
            m_components.erase( it );
        }
    }
}

void world::cleanup()
{
    for( entity* e : m_entities_to_remove )
    {
        remove_entity( *e );
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
