#include "utility.hpp"
#include <cstddef>
#include <memory>
#include <type_traits>

namespace my {
template <class T> class optional {
public:
    using value_type = T;

private:
    union {
        std::byte m_none;
        T m_some;
    };
    bool m_is_some;


public:
    // constructors
    constexpr optional() noexcept : m_is_some{false} {}

    constexpr optional(std::nullopt_t) noexcept : m_is_some{false} {}

    constexpr optional(const optional &other) : m_is_some{other.m_is_some} {
        if (m_is_some) {
            std::construct_at(std::addressof(m_some), other.m_some);
        }
    }

    constexpr optional(optional &&other) noexcept : m_is_some{other.m_is_some} {
        if (m_is_some) {
            std::construct_at(std::addressof(m_some), std::move(other.m_some));
        }
    }

    template <class U>
    constexpr optional(const optional<U> &other)
        requires std::is_constructible_v<T, const U &>
    : m_is_some{other.m_is_some} {
        if (m_is_some) {
            std::construct_at(std::addressof(m_some), other.m_some);
        }
    }

    template <class U>
    constexpr optional(optional<U> &&other)
        requires std::is_constructible_v<T, U>
    : m_is_some{other.m_is_some} {
        if (m_is_some) {
            std::construct_at(std::addressof(m_some), std::move(other.m_some));
        }
    }

    template <class... Args>
    constexpr explicit optional(std::in_place_t, Args &&...args)
        requires std::is_constructible_v<T, Args...>
    : m_is_some{true} {
        std::construct_at(std::addressof(m_some), std::forward<Args>(args)...);
    }

    template <class U, class... Args>
    constexpr explicit optional(std::in_place_t,
                                std::initializer_list<U> ilist,
                                Args &&...args)
        requires std::is_constructible_v<T, std::initializer_list<U> &, Args...>
    : m_is_some{true} {
        std::construct_at(std::addressof(m_some), ilist, std::forward<Args>(args)...);
    }

    template <class U = std::remove_cv_t<T>>
    constexpr optional(U &&value)
        requires std::is_constructible_v<T, U>
    : m_is_some{true} {
        std::construct_at(std::addressof(m_some), std::move(value));
    }

    constexpr ~optional() {
        if (m_is_some) {
            m_some.~T();
            m_is_some = false;
        }
    }

    constexpr const T *operator->() const noexcept;
    constexpr explicit operator bool() const noexcept { return m_is_some; }
    constexpr bool has_value() const noexcept { return m_is_some; }

    template <class U = std::remove_cv_t<T>>
    constexpr T value_or(U &&default_value) const & {
        return m_is_some ? m_some : static_cast<T>(std::forward<U>(default_value));
    }

}; // class optional
} // namespace my
