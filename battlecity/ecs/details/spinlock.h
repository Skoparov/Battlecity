#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>

namespace ecs
{

class lockable
{
public:
    lockable() = default;
    // required due to necessity to initially move a guarded component
    lockable( lockable&& ){}

    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;

private:
    std::atomic_flag m_flag{ ATOMIC_FLAG_INIT };
};

}// ecs

#endif
