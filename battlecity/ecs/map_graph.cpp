#include "map_graph.h"

#include "components.h"

static const int value_traversible{ 1 };
static const int value_non_traversible{ 5 };

inline uint qHash( const QRect& r)
{
    return qHash( r.left() ) + qHash( r.top() ) + qHash( r.width() ) + qHash( r.bottom() );
}

template< typename T >
T abs_diff( T l, T r ) noexcept
{
    return  l >= r? l - r : r - l;
}

namespace game
{

map_tile_node::map_tile_node( ecs::entity& e ) noexcept : m_tile_entity( &e ){}

void map_tile_node::set_left( map_tile_node& node ) noexcept
{
    m_left = &node;
}

void map_tile_node::set_right( map_tile_node& node ) noexcept
{
    m_right = &node;
}

void map_tile_node::set_top( map_tile_node& node ) noexcept
{
    m_top = &node;
}

void map_tile_node::set_bottom( map_tile_node& node ) noexcept
{
    m_bottom = &node;
}

map_tile_node* map_tile_node::get_left() const noexcept
{
    return m_left;
}

map_tile_node* map_tile_node::get_right() const noexcept
{
    return m_right;
}

map_tile_node* map_tile_node::get_top() const noexcept
{
    return m_top;
}

map_tile_node* map_tile_node::get_bottom() const noexcept
{
    return m_bottom;
}

ecs::entity& map_tile_node::get_entity() noexcept
{
    return *m_tile_entity;
}

const ecs::entity& map_tile_node::get_entity() const noexcept
{
    return *m_tile_entity;
}

bool map_tile_node::is_traversible() const noexcept
{
    return m_tile_entity->has_component< component::non_traversible_tile >() ||
           m_tile_entity->has_component< component::non_traversible_object >();
}

const QRect& map_tile_node::get_rect() const noexcept
{
    return m_tile_entity->get_component< component::geometry >().get_rect();
}

size_t get_row( size_t node, size_t col_count ) noexcept
{
    return node / col_count;
}

size_t get_col( size_t node, size_t col_count ) noexcept
{
    return node % col_count;
}

map_tile_node& get_node( int row, int col, size_t col_count, map_graph& graph ) noexcept
{
    return *graph[ row * col_count +col ];
}

bool is_adjacent( size_t node1, size_t node2, size_t col_count ) noexcept
{
    size_t node1_row{ get_row( node1, col_count ) };
    size_t node2_row{ get_row( node2, col_count ) };
    size_t node1_col{ get_col( node1, col_count ) };
    size_t node2_col{ get_col( node2, col_count ) };

    return ( node1_row == node2_row && abs_diff( node1_col, node2_col ) == 1 ) ||
           ( node1_col == node2_col && abs_diff( node1_row, node2_row ) == 1 );
}

int get_dist_adjacent( size_t to, const map_graph& graph ) noexcept
{
    return graph[ to ]->get_entity().has_component< component::non_traversible_tile >()?
                value_non_traversible : value_traversible;
}

int min_distance( const std::vector< int >& dist, const std::vector< bool >& spt_set, int max ) noexcept
{
   int min{ max };
   int min_index{ 0 };

   for( size_t i{ 0 }; i < spt_set.size(); ++i )
   {
        if( !spt_set[ i ] && dist[ i ] <= min )
        {
            min = dist[ i ];
            min_index = static_cast< int >( i );
        }
   }

   return min_index;
}

void construct_path( size_t from,
                     size_t to,
                     const std::vector< int >& parents,
                     map_graph& graph,
                     map_paths& paths )
{
    const map_tile_node& from_node = *graph[ from ];
    const map_tile_node& to_node = *graph[ to ];

    if( paths.count( map_path_key{ to_node.get_rect(), from_node.get_rect() } ) )
    {
        return;
    }

    map_path& path =
            paths[ map_path_key{ from_node.get_rect(), to_node.get_rect() } ];


    int parent{ parents[ to ] };
    while( parent != -1 )
    {
        path.push_front( graph[ parent ].get() );
        parent = parents[ parent ];
    }
}

void dijkstra( size_t from, map_graph& graph, size_t col_count, map_paths& paths )
{
    static std::vector< bool > spt_set;
    static std::vector< int > distances;
    static std::vector< int > parents;
    if( spt_set.empty() )
    {
        spt_set.resize( graph.size() );
        distances.resize( graph.size() );
        parents.resize( graph.size() );
    }

    for( size_t i{ 0 }; i < graph.size(); ++i )
    {
        parents[ i ] = -1;
        spt_set[ i ] = false;
        distances[ i ] = graph.size() + 1;
    }


    distances[ from ] = 0;

    for( size_t count{ 0 }; count < graph.size() - 1; ++count )
    {
        int min_index{ min_distance( distances, spt_set, graph.size() ) };
        spt_set[ min_index ] = true;

         for( size_t node{ 0 }; node < graph.size(); ++node )
         {
             if( !spt_set[ node ] && is_adjacent( min_index, node, col_count ) )
             {
                 int dist{ distances[ min_index ] + get_dist_adjacent( node, graph ) };
                 if( dist < distances[ node ] )
                 {
                     parents[ node ] = min_index;
                     distances[ node ] = dist;
                 }
             }
         }
    }

    for( size_t to{ 1 }; to < graph.size(); ++to )
    {
        construct_path( from, to, parents, graph, paths );
    }
}

map_tile_node& create_map_node( ecs::entity& e, int row, int col, int col_count, map_graph& graph )
{
    graph.emplace_back( std::make_unique< map_tile_node >( e ) );
    auto& node = graph.back();

    if( row > 0 )
    {
        map_tile_node& top_node = get_node( row - 1, col, col_count, graph );
        node->set_top( top_node );
        top_node.set_bottom( *node );
    }

    if( col > 0 )
    {
        map_tile_node& left_node = get_node( row, col - 1, col_count, graph );
        node->set_left( left_node );
        left_node.set_right( *node );
    }

    return *node;
}

}// game
