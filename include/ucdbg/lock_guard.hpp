#pragma once

#include <concepts>
#include <ucdbg/event_helpers.hpp>

namespace ucdbg {
namespace internal {

template <class T>
concept Lockable = requires(T& t) {
    { t.lock() } -> std::same_as<void>;
    { t.unlock() } -> std::same_as<void>;
};

template<Lockable L>
class LockGuard {
public:
    explicit LockGuard(L& lockable,uint64_t lock_id = 0) 
        : lockable_(lockable), 
        lock_id_(lock_id ? lock_id : reinterpret_cast<uint64_t>(&lockable)) {        
        lockable_.lock();
        auto event = internal::make_concurrency_event(EventType::LockAcquire, lock_id_);
    }

    ~LockGuard() noexcept {
        lockable_.unlock();
        auto event = internal::make_concurrency_event(EventType::LockRelease, lock_id_);
    }



    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;
    LockGuard(LockGuard&&) = delete;
    LockGuard& operator=(LockGuard&&) = delete;

private:
    L& lockable_;
    uint64_t lock_id_;
};    

}  // namespace internal 
} // namespace ucdbg
