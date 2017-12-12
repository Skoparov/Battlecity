#include "id_engine.h"
#include <random>

namespace ecs
{

numeric_id generate_numeric_id()
{
    static std::mt19937 rng{ std::random_device{}() };
    static std::uniform_int_distribution< std::mt19937::result_type > dist{
                std::numeric_limits< numeric_id >::min(),
                std::numeric_limits< numeric_id >::max() };

    numeric_id id = dist( rng );
    while( id == INVALID_NUMERIC_ID )
    {
        id = dist( rng );
    }

    return id;
}

}// ecs
