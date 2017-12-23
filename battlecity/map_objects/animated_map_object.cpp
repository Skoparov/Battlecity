#include "animated_map_object.h"

#include  "ecs/components.h"

namespace game
{

uint32_t animated_map_object::get_loops_num() const noexcept
{
    const component::animation_info& ai = m_entity->get_component_unsafe< component::animation_info >();
        return ai.get_loops_num();
}

uint32_t animated_map_object::get_frame_rate() const noexcept
{
    const component::animation_info& ai = m_entity->get_component_unsafe< component::animation_info >();
    return ai.get_frame_rate();
}

uint32_t animated_map_object::get_frames_num() const noexcept
{
    const component::animation_info& ai = m_entity->get_component_unsafe< component::animation_info >();
    return ai.get_frames_num();
}

uint64_t animated_map_object::get_duration() const noexcept
{
    const component::animation_info& ai = m_entity->get_component_unsafe< component::animation_info >();
    return ai.get_duration().count();
}

}// game
