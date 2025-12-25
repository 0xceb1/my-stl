#pragma once
#include <print>
#include <type_traits>
#include <utility>

namespace my {

template<class T1, class T2>
class pair {
public:
    T1 first;
    T2 second;
    // For debug purpose only
    void show() {
        std::println("my::pair({}, {})", first, second);
    }

    // constructors
    constexpr pair()
        requires std::is_default_constructible_v<T1> && std::is_default_constructible_v<T2>
     = default;
    constexpr pair(const T1 x, const T2& y)
        requires std::is_copy_constructible_v<T1> && std::is_copy_constructible_v<T2>
    : first{x}, second{y} {}

    template<class U1 = T1, class U2 = T2>
    constexpr pair(U1&& x, U2&& y)
        requires std::is_constructible_v<T1, U1> && std::is_constructible_v<T1, U1>
    : first{std::forward<U1>(x)}, second{std::forward<U2>(y)} {}

    template<class U1, class U2>
    constexpr pair(pair<U1, U2>& p)
        requires std::is_constructible_v<T1, U1&> && std::is_constructible_v<T2, U2&>
    : first{p.first}, second{p.second} {}

    template<class U1, class U2>
    constexpr pair(const pair<U1, U2>& p)
        requires std::is_constructible_v<T1, const U1&> && std::is_constructible_v<T2, const U2&>
    : first{p.first}, second{p.second} {}

    template<class U1, class U2>
    constexpr pair(pair<U1, U2>&& p)
        requires std::is_constructible_v<T1, U1> && std::is_constructible_v<T2, U2>
    : first{std::forward<U1>(p.first)}, second{std::forward<U2>(p.second)} {}

    template<class U1, class U2>
    constexpr pair(const pair<U1, U2>&& p)
        requires std::is_constructible_v<T1, U1> && std::is_constructible_v<T2, U2>
    : first{std::forward<const U1>(p.first)}, second{std::forward<const U2>(p.second)} {}

    pair(const pair& p) = default;

    pair(pair&& p) = default;

    // copy assign operator
    constexpr pair& operator=(const pair& other)
        requires std::is_copy_constructible_v<T1> && std::is_copy_constructible_v<T2>
    = default;

    constexpr const pair& operator=(const pair& other) const
        requires std::is_copy_constructible_v<T1> && std::is_copy_constructible_v<T2>
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    template<class U1, class U2>
    constexpr pair& operator=(const pair<U1, U2>& other) 
        requires std::is_assignable_v<T1&, const U1&> && std::is_assignable_v<T2&, const U2&>
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    template<class U1, class U2>
    constexpr const pair& operator=(const pair<U1, U2>& other) const
        requires std::is_assignable_v<T1&, const U1&> && std::is_assignable_v<T2&, const U2&>
    {
        first = other.first;
        second = other.second;
        return *this;
    }

    constexpr pair& operator=(pair&& other)
        requires std::is_move_assignable_v<T1> && std::is_move_assignable_v<T1>
    = default;

    constexpr const pair& operator=(pair&& other) const
        requires std::is_assignable_v<const T1&, T1> && std::is_assignable_v<const T2&, T2>
    {
        first = std::move(other.first);
        second = std::move(other.second);
        return *this;
    }

    template<class U1, class U2>
    constexpr pair& operator=(pair<U1, U2>&& p)
        requires std::is_assignable_v<T1&, U1> && std::is_assignable_v<T2&, U2>
    {
        first = std::forward<U1>(p.first);
        second = std::forward<U2>(p.second);
        return *this;
    }

    template<class U1, class U2>
    constexpr const pair& operator=(pair<U1, U2>&& p) const
        requires std::is_assignable_v<const T1&, U1> && std::is_assignable_v<const T2&, U2>
    {
        first = std::forward<U1>(p.first);
        second = std::forward<U2>(p.second);
        return *this;
    }
}; // class pair

template<class T1, class T2>
constexpr pair<std::unwrap_ref_decay_t<T1>, std::unwrap_ref_decay_t<T2>>
make_pair(T1&& x, T2&& y) {
    return { std::forward<T1>(x), std::forward<T2>(y) };
}

template< class T1, class T2, class U1, class U2 >
bool operator==(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template<class T1, class T2, class U1, class U2>
constexpr auto operator<=>(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs) {
    if (auto c = lhs.first <=> rhs.first; c != 0) return c;
    return lhs.second <=> rhs.second;
}
} // namespace my
