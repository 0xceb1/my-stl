#pragma once
#include <cstddef>
#include <type_traits>

namespace my {

template<class T>
class allocator {
public:
    using value_type                             = T;
    using size_type                              = std::size_t;
    using difference_type                        = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;

    // constructors
    constexpr allocator() noexcept = default;
    constexpr allocator(const allocator& other) noexcept : allocator() {}
    template<class U>
    constexpr allocator(const allocator<U>& other) noexcept : allocator() {}
    // destructor
    constexpr ~allocator() = default;

    constexpr T* allocate(size_type n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    constexpr std::allocation_result<T*, size_type>
    allocate_at_least(size_type n) {
        return { allocate(n), n };
    }

    constexpr void deallocate(T* p, size_type n) {
        ::operator delete(p, n * sizeof(T));
    }
}; // class allocator

template< class T1, class T2 >
constexpr bool operator==( const allocator<T1>& lhs, const allocator<T2>& rhs ) noexcept {
    return true;
}
} // namespace my
