#ifndef MAP_GRAPH_H
#define MAP_GRAPH_H

#include <vector>
#include <deque>

#include <QRect>
#include <QHash>

#include "framework/entity.h"

namespace game
{

class map_tile_node
{
public:
    explicit map_tile_node( ecs::entity& e ) noexcept;

    void set_left( map_tile_node& node ) noexcept;
    void set_right( map_tile_node& node ) noexcept;
    void set_top( map_tile_node& node ) noexcept;
    void set_bottom( map_tile_node& node ) noexcept;

    map_tile_node* get_left() const noexcept;
    map_tile_node* get_right() const noexcept;
    map_tile_node* get_top() const noexcept;
    map_tile_node* get_bottom() const noexcept;

    ecs::entity& get_entity() noexcept;
    const ecs::entity& get_entity() const noexcept;

    bool is_traversible() const noexcept;
    const QRect& get_rect() const noexcept;

private:
    map_tile_node* m_left{ nullptr };
    map_tile_node* m_right{ nullptr };
    map_tile_node* m_top{ nullptr };
    map_tile_node* m_bottom{ nullptr };
    ecs::entity* m_tile_entity{ nullptr };
};

using map_graph = std::vector< std::unique_ptr< map_tile_node > >;
using map_path = std::deque< map_tile_node* >;
using map_path_key = QPair< QRect, QRect >;
using map_paths = QHash< map_path_key, map_path >;

map_tile_node& create_map_node( ecs::entity& e, int row, int col, int col_count, map_graph& graph );
void dijkstra( size_t from, map_graph& graph, size_t col_count, map_paths& paths );

}// game

#endif
