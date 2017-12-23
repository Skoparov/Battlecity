#ifndef ECS_ATOMIC_LOCKS_H
#define ECS_ATOMIC_LOCKS_H

#include <chrono>
#include <atomic>

#include "rw_lock_modes.h"

namespace ecs
{

class spinlock final
{
public:
    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;

private:
    std::atomic_flag m_flag{ ATOMIC_FLAG_INIT };
};

class rw_spinlock final
{
public:
    void lock( const lock_mode& mode ) noexcept;
    bool try_lock( const lock_mode& mode ) noexcept;

    template< typename rep, typename period >
    bool try_lock_for( const lock_mode& mode, const std::chrono::duration< rep, period >& timeout ) noexcept
    {
        using namespace std::chrono;
        using clock = high_resolution_clock;
        using timeout_type = duration< rep, period >;

        auto start = clock::now();
        while( duration_cast< timeout_type >( clock::now() - start ) < timeout )
        {
            if( try_lock( mode ) )
            {
                return true;
            }
        }

        return false;
    }

    void unlock( const lock_mode& mode ) noexcept;

private:
    void lock_read() noexcept;
    bool try_lock_read() noexcept;

    void lock_write() noexcept;
    bool try_lock_write() noexcept;

    void unlock_read() noexcept;
    void unlock_write() noexcept;

private:
    std::atomic_uint_fast32_t m_lock{ 0 };
};

}// ecs

#endif
