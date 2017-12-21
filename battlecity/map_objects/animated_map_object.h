#ifndef ANIMATED_MAP_OBJECT_H
#define ANIMATED_MAP_OBJECT_H

#include "graphics_map_object.h"

namespace game
{

class animated_map_object : public graphics_map_object
{
    Q_OBJECT

public:
    using graphics_map_object::graphics_map_object;

    uint32_t get_loops_num() const noexcept;
    uint32_t get_frame_rate() const noexcept;
    uint32_t get_frames_num() const noexcept;
    uint64_t get_duration() const noexcept;

    Q_PROPERTY( int loops_num READ get_loops_num CONSTANT )
    Q_PROPERTY( int frame_rate READ get_frame_rate CONSTANT )
    Q_PROPERTY( int frames_num READ get_frames_num CONSTANT )
    Q_PROPERTY( int duration READ get_duration CONSTANT )
};

}// game

#endif
