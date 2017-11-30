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

entity& world::create_entity()
{
    entity_id id{ generate_entity_id( m_entities ) };
    auto res = m_entities.emplace( id, entity{ id } );
    return res.first->second;
}

void world::schedule_destroy_entity( entity& e )
{
    m_entities_to_remove.emplace_back( e.id() );
}

void world::add_system( system& system )
{
    m_systems.emplace( &system );
}

void world::schedule_remove_system( system& system )
{
    m_systems_to_remove.emplace_back( &system );
}

void world::cleanup()
{
    for( entity_id id : m_entities_to_remove )
    {
        m_entities.erase( id );
    }

    for( system* system : m_systems_to_remove )
    {
        m_systems.erase( system );
    }

    m_entities_to_remove.clear();
    m_systems_to_remove.clear();
}

system::system( world &world ) noexcept: m_world( world ){}

}// ecs
