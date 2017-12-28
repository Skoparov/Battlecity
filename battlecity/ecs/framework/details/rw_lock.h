#ifndef ECS_READ_WRITE_LOCK_H
#define ECS_READ_WRITE_LOCK_H

#include <atomic>
#include <condition_variable>

#include "rw_lock_modes.h"

namespace ecs
{

// A writer has priority over readers,
// So if an attempt to write lock happens
// No new readers would be allowed to lock
// Until the writer is done with it's changes

class rw_lock
{
public:
    rw_lock() = default;
    rw_lock( const rw_lock& ) = delete;
    rw_lock( rw_lock&& ) = delete;
    rw_lock& operator=(const rw_lock&) = delete;
    rw_lock& operator=( rw_lock&& ) = delete;

    void lock( const lock_mode& mode );
    bool try_lock( const lock_mode& mode );

    template< typename rep, typename period >
    bool try_lock_for( const lock_mode& mode,
                       const std::chrono::duration< rep, period >& timeout )
    {
        std::unique_lock< std::mutex > l{ m_mutex };
        bool result{ false };

        if( mode == lock_mode::read )
        {
            result = m_read_cv.wait_for( l, timeout, [ this ]{ return !m_writer_waiting; } );
            ++m_readers_num;
        }
        else
        {
            m_writer_waiting = true;
            result = m_write_cv.wait( l, [ this ]{ return !m_readers_num; } );
        }

        return result;
    }

    void unlock( const lock_mode& mode );

private:
    void lock_read();
    void lock_write();

    bool try_lock_read();
    bool try_lock_write();

    void unlock_read();
    void unlock_write();

private:
    std::mutex m_mutex;
    std::condition_variable m_read_cv;
    std::condition_variable m_write_cv;

    size_t m_readers_num{ 0 };
    bool m_writer_waiting{ false };
};

}// ecs

#endif
