#include "rw_lock.h"

namespace ecs
{

void rw_lock::lock( const lock_mode& mode )
{
    if( mode == lock_mode::read )
    {
        lock_read();
    }
    else
    {
        lock_write();
    }
}

bool rw_lock::try_lock( const lock_mode& mode )
{
    return mode == lock_mode::read? try_lock_read() : try_lock_write();
}

void rw_lock::unlock( const lock_mode& mode )
{
    if( mode == lock_mode::read )
    {
        unlock_read();
    }
    else
    {
        unlock_write();
    }
}

void rw_lock::lock_read()
{
    std::unique_lock< std::mutex > l{ m_mutex };

    m_read_cv.wait( l, [ this ]{ return !m_writer_waiting; } );
    ++m_readers_num;
}

void rw_lock::lock_write()
{
    std::unique_lock< std::mutex > l{ m_mutex };

    m_writer_waiting = true;
    m_write_cv.wait( l, [ this ]{ return !m_readers_num; } );
}

bool rw_lock::try_lock_read()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    bool result{ !m_writer_waiting };
    if( result )
    {
        ++m_readers_num;
    }

    return result;
}

bool rw_lock::try_lock_write()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    bool result{ !m_readers_num };
    if( result )
    {
        m_writer_waiting = true;
    }

    return result;
}

void rw_lock::unlock_read()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    --m_readers_num;

    if( !m_readers_num )
    {
        m_write_cv.notify_all();
    }
}

void rw_lock::unlock_write()
{
    std::lock_guard< std::mutex > l{ m_mutex };

    m_writer_waiting = false;
    m_read_cv.notify_all();
}

}// ecs
