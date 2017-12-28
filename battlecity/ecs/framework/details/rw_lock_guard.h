#ifndef ECS_RW_LOCK_GUARD_H
#define ECS_RW_LOCK_GUARD_H

#include <chrono>
#include <atomic>

#include "rw_lock_modes.h"

namespace ecs
{

enum class lock_policy{ instant, defer };

template< typename lock_type >
class rw_lock_guard
{
public:
    explicit rw_lock_guard( lock_type& lock,
                            const lock_mode& mode,
                            const lock_policy& policy = lock_policy::instant ):
        m_lock( lock ),
        m_mode( mode )
    {
        if( policy == lock_policy::instant )
        {
            this->lock();
        }
    }

    rw_lock_guard( const rw_lock_guard& ) = delete;
    rw_lock_guard( rw_lock_guard&& ) = delete;
    rw_lock_guard& operator=(const rw_lock_guard&) = delete;
    rw_lock_guard& operator=( rw_lock_guard&& ) = delete;
    ~rw_lock_guard()
    {
        if( m_owns_lock )
        {
            m_lock.unlock( m_mode );
        }
    }

    void lock()
    {
        m_lock.lock( m_mode );
        m_owns_lock = true;
    }

    bool try_lock()
    {
        m_owns_lock = m_lock.try_lock( m_mode );
        return m_owns_lock;
    }

    template< typename rep, typename period >
    bool try_lock_for( const std::chrono::duration< rep, period >& timeout )
    {
        m_owns_lock = m_lock.try_lock_for( m_mode, timeout );
        return m_owns_lock;
    }

    void unlock()
    {
        m_lock.unlock( m_mode );
        m_owns_lock = false;
    }

    bool owns_lock() const noexcept{ return m_owns_lock; }
    void release() noexcept{ m_owns_lock = false; }

private:
    lock_type& m_lock;
    lock_mode m_mode;
    std::atomic_bool m_owns_lock{ false };
};

}// ecs

#endif
