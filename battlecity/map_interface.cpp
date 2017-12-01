#include "map_interface.h"

namespace game
{

map_interface::map_interface( ecs::world& world, QObject* parent ) :
    QObject( parent ),
    m_world( world ){}

void map_interface::load_level(uint32_t level)
{
    m_map_data = read_map_file( QString{ ":/maps/map_%1" }.arg( level ), m_world );
}

void map_interface::clear()
{
    m_map_data = {};
}

}// game
