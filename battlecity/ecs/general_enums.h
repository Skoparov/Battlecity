#ifndef GENERAL_ENUMS_H
#define GENERAL_ENUMS_H

namespace game
{

enum class tile_type{ wall, iron_wall, empty };
enum class alignment{ player, enemy };
enum class movement_direction{ left, right, up, down, none };
enum class has_infinite_lifes{ yes, no };
enum class object_type{ tile,
                        player_base,
                        player_tank,
                        enemy_tank,
                        projectile,
                        respawn_point,
                        frag,
                        explosion };

}// game

#endif
