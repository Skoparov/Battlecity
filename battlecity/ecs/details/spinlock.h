#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <atomic>

namespace game
{

namespace _detail
{

class spinlock final
{
public:
    spinlock() = default;
    // required due to necessity to initially move a guarded component
    spinlock( spinlock&& ){}

    void lock() noexcept;
    bool try_lock() noexcept;
    void unlock() noexcept;

private:
    std::atomic_flag m_flag{ ATOMIC_FLAG_INIT };
};

}// _detail

}// game

#endif
