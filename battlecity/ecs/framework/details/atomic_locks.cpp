#include "atomic_locks.h"

#include <cassert>

namespace ecs
{

void spinlock::lock() noexcept
{
    while( m_flag.test_and_set( std::memory_order_acquire ) );
}

bool spinlock::try_lock() noexcept
{
    return !m_flag.test_and_set( std::memory_order_acquire );
}

void spinlock::unlock() noexcept
{
    m_flag.clear( std::memory_order_release );
}

//

void rw_spinlock::lock( const lock_mode& mode ) noexcept
{
    return mode == lock_mode::read? lock_read() : lock_write();
}

bool rw_spinlock::try_lock( const lock_mode& mode) noexcept
{
    return mode == lock_mode::read? try_lock_read() : try_lock_write();
}

void rw_spinlock::unlock( const lock_mode& mode ) noexcept
{
    return mode == lock_mode::read? unlock_read() : unlock_write();
}

void rw_spinlock::lock_read() noexcept
{
    while( true )
    {
        uint_fast32_t old_lock{ m_lock & 0x7fffffff };
        uint_fast32_t new_lock{ old_lock + 1 };

        if( m_lock.compare_exchange_weak( old_lock, new_lock, std::memory_order_acquire ) )
        {
            return;
        }
    }
}

bool rw_spinlock::try_lock_read() noexcept
{
    uint_fast32_t old_lock{ m_lock & 0x7fffffff };
    uint_fast32_t new_lock{ old_lock + 1 };

    return m_lock.compare_exchange_strong( old_lock, new_lock, std::memory_order_acquire );
}

void rw_spinlock::lock_write() noexcept
{
    while( true )
    {
        uint_fast32_t old_lock{ m_lock & 0x7fffffff };
        uint_fast32_t new_lock{ old_lock | 0x80000000 };

        if( m_lock.compare_exchange_weak( old_lock, new_lock, std::memory_order_acquire ) )
        {
            while( m_lock & 0x7fffffff );// wait for readers to finish
            return;
        }
    }
}

bool rw_spinlock::try_lock_write() noexcept
{
    uint_fast32_t expected{ 0 };
    return m_lock.compare_exchange_strong( expected, 0x80000000, std::memory_order_acquire );
}

void rw_spinlock::unlock_read() noexcept
{
    assert( m_lock & 0x7fffffff );

    while( true )
    {
        uint_fast32_t old_lock{ m_lock };
        uint_fast32_t new_lock{ m_lock - 1 };

        if( m_lock.compare_exchange_weak( old_lock, new_lock, std::memory_order_acquire ) )
        {
            return;
        }
    }
}

void rw_spinlock::unlock_write() noexcept
{
    assert( m_lock == 0x80000000 );
    m_lock = 0;
}

}//ecs
