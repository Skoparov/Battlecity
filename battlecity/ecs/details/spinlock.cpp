#include "spinlock.h"

namespace ecs
{

void lockable::lock() noexcept
{
    while( m_flag.test_and_set( std::memory_order_acquire ) );
}

bool lockable::try_lock() noexcept
{
    return !m_flag.test_and_set( std::memory_order_acquire );
}

void lockable::unlock() noexcept
{
    m_flag.clear( std::memory_order_release );
}

}// ecs
