#include "utility.hpp"
#include <concepts>
#include <cstddef>
#include <cassert>
#include <memory>
#include <optional>
#include <type_traits>
#include <format>
#include <utility>

namespace my {

namespace {

template<class T, class U>
concept can_construct_from_optional =
    std::is_constructible_v<T, std::optional<U>&> ||
    std::is_constructible_v<T, const std::optional<U>&> ||
    std::is_constructible_v<T, std::optional<U>&&> ||
    std::is_constructible_v<T, const std::optional<U>&&>;

template<class T, class U>
concept can_convert_to_optional =
    std::is_convertible_v<std::optional<U>&, T> ||
    std::is_convertible_v<const std::optional<U>&, T> ||
    std::is_convertible_v<std::optional<U>&&, T> ||
    std::is_convertible_v<const std::optional<U>&&, T>;

template<class T, class U>
concept can_assign_from_optional =
    std::is_assignable_v<T&, std::optional<U>&> ||
    std::is_assignable_v<T&, const std::optional<U>&> ||
    std::is_assignable_v<T&, std::optional<U>&&> ||
    std::is_assignable_v<T&, const std::optional<U>&&>;

} // anonymous namespace

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
        requires std::is_constructible_v<T, const U &> &&
                 (!can_construct_from_optional<T, U>) &&
                 (!can_convert_to_optional<T, U>)
    : m_is_some{other.m_is_some} {
        if (m_is_some) {
            std::construct_at(std::addressof(m_some), other.m_some);
        }
    }

    template <class U>
    constexpr optional(optional<U> &&other)
        requires std::is_constructible_v<T, U> &&
                 (!can_construct_from_optional<T, U>) &&
                 (!can_convert_to_optional<T, U>)
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

    // destructors
    constexpr ~optional() {
        reset();
    }

    // copy assignment operator
    constexpr optional& operator=(std::nullopt_t) noexcept {
        reset();
        return *this;
    }

    constexpr optional& operator=(const optional& other) {
        if (other.m_is_some) {
            m_some = other.m_some;
            m_is_some = true;
        } else {
            reset();
        }
        return *this;
    }

    constexpr optional& operator=(optional&& other) noexcept {
        if (other.m_is_some) {
            m_some = std::move(other.m_some);
            m_is_some = true;
        } else {
            reset();
        }
        return *this;
    }

    template<class U>
    constexpr optional& operator=(const optional<U>& other)
        requires std::is_constructible_v<T, const U&> &&
                 std::is_assignable_v<T&, const U&> &&
                 (!can_construct_from_optional<T, U>) &&
                 (!can_convert_to_optional<T, U>) &&
                 (!can_assign_from_optional<T, U>)
    {
        if (other.m_is_some) {
            m_some = other.m_some;
            m_is_some = true;
        } else {
            reset();
        }
        return *this;
    }

    template<class U>
    constexpr optional& operator=(optional<U>&& other)
        requires std::is_constructible_v<T, U> &&
                 std::is_assignable_v<T&, U> &&
                 (!can_construct_from_optional<T, U>) &&
                 (!can_convert_to_optional<T, U>) &&
                 (!can_assign_from_optional<T, U>)
    {
        if (other.m_is_some) {
            m_some = std::move(other.m_some);
            m_is_some = true;
        } else {
            reset();
        }
        return *this;
    }

    template<class U = std::remove_cv_t<T>>
    constexpr optional& operator=(U&& value)
        requires std::is_constructible_v<T, U> &&
                 std::is_assignable_v<T&, U> &&
                 (!std::same_as<std::remove_cvref_t<U>, std::optional<T>>)
    {
        m_some = std::forward<U>(value);
        m_is_some = true;
        return *this;
    }

    constexpr const T* operator->() const noexcept {
        assert(m_is_some && "optional must hava a value to call -> operator");
        return std::addressof(m_some);
    }

    constexpr T* operator->() noexcept {
        assert(m_is_some && "optional must hava a value to call -> operator");
        return std::addressof(m_some);
    }

    constexpr const T& operator*() const& noexcept {
        assert(m_is_some && "optional must hava a value to call * operator");
        return m_some;
    }

    constexpr T& operator*() & noexcept {
        assert(m_is_some && "optional must hava a value to call * operator");
        return m_some;
    }

    constexpr const T&& operator*() const&& noexcept {
        assert(m_is_some && "optional must hava a value to call * operator");
        return std::move(m_some);
    }

    constexpr T&& operator*() && noexcept {
        assert(m_is_some && "optional must hava a value to call * operator");
        return std::move(m_some);
    }

    constexpr explicit operator bool() const noexcept { return m_is_some; }
    constexpr bool has_value() const noexcept { return m_is_some; }

    constexpr T& value() & {
        if (!m_is_some) {
            throw std::bad_optional_access();
        }
        return m_some;
    }

    constexpr const T& value() const & {
        if (!m_is_some) {
            throw std::bad_optional_access();
        }
        return m_some;
    }

    constexpr T&& value() && {
        if (!m_is_some) {
            throw std::bad_optional_access();
        }
        return std::move(m_some);

    }
    constexpr const T&& value() const && {
        if (!m_is_some) {
            throw std::bad_optional_access();
        }
        return std::move(m_some);
    }

    template <class U = std::remove_cv_t<T>>
    constexpr T value_or(U &&default_value) const & {
        return m_is_some ? m_some : static_cast<T>(std::forward<U>(default_value));
    }

    template <class U = std::remove_cv_t<T>>
    constexpr T value_or(U &&default_value) && {
        return m_is_some ? std::move(m_some) : static_cast<T>(std::forward<U>(default_value));
    }

    // monadic operations
    template<class F>
    constexpr auto and_then(F&& f) & {
        if (m_is_some)
            return std::invoke(std::forward<F>(f), m_some);
        else
            return std::remove_cvref_t<std::invoke_result_t<F, T&>>{};
    }

    template<class F>
    constexpr auto and_then(F&& f) const& {
        if (m_is_some)
            return std::invoke(std::forward<F>(f), m_some);
        else
            return std::remove_cvref_t<std::invoke_result_t<F, const T&>>{};
    }

    template<class F>
    constexpr auto and_then(F&& f) && {
        if (m_is_some)
            return std::invoke(std::forward<F>(f), std::move(m_some));
        else
            return std::remove_cvref_t<std::invoke_result_t<F, T>>{};
    }

    template<class F>
    constexpr auto and_then(F&& f) const&& {
        if (m_is_some)
            return std::invoke(std::forward<F>(f), std::move(m_some));
        else
            return std::remove_cvref_t<std::invoke_result_t<F, const T>>{};
    }

    template<class F>
    constexpr auto transform(F&& f) & {
        using U = std::remove_cv_t<std::invoke_result_t<F, T&>>;
        if (m_is_some)
            return optional<U>{ std::invoke(std::forward<F>(f), m_some) };
        else
            return optional<U>{};
    }

    template<class F>
    constexpr auto transform(F&& f) const& {
        using U = std::remove_cv_t<std::invoke_result_t<F, T&>>;
        if (m_is_some)
            return optional<U>{ std::invoke(std::forward<F>(f), m_some) };
        else
            return optional<U>{};
    }

    template<class F>
    constexpr auto transform(F&& f) && {
        using U = std::remove_cv_t<std::invoke_result_t<F, T&>>;
        if (m_is_some)
            return optional<U>{ std::invoke(std::forward<F>(f), std::move(m_some)) };
        else
            return optional<U>{};
    }

    template<class F>
    constexpr auto transform(F&& f) const&& {
        using U = std::remove_cv_t<std::invoke_result_t<F, T&>>;
        if (m_is_some)
            return optional<U>{ std::invoke(std::forward<F>(f), std::move(m_some)) };
        else
            return optional<U>{};
    }

    template<class F>
    constexpr optional or_else(F&& f) const& {
        return m_is_some ? m_some : std::forward<F>(f)();
    }

    template<class F>
    constexpr optional or_else(F&& f) && {
        return m_is_some ? std::move(m_some) : std::forward<F>(f)();
    }

    // modifiers
    constexpr void swap(optional& other) {
        if (this != &other) {
            if (m_is_some && other.m_is_some) {
                std::swap(m_some, other.m_some);
            } else if (!m_is_some && other.m_is_some) {
                std::construct_at(std::addressof(m_some), std::move(other.m_some));
                other.reset();
                m_is_some = true;
            } else if (m_is_some && !other.m_is_some) {
                std::construct_at(std::addressof(other.m_some), std::move(m_some));
                reset();
                other.m_is_some = true;
            } else { return; }
        }
    }

    template<class... Args>
    constexpr T& emplace(Args&&... args) {
        if (m_is_some) {
            m_some.~T();
        }
        std::construct_at(std::addressof(m_some), std::forward<Args>(args)...);
        m_is_some = true;
        return m_some;
    }

    template<class U, class... Args>
    T& emplace(std::initializer_list<U> ilist, Args&&... args) {
        if (m_is_some) {
            m_some.~T();
        }
        std::construct_at(std::addressof(m_some), ilist, std::forward<Args>(args)...);
        m_is_some = true;
        return m_some;
    }

    constexpr void reset() noexcept {
        if (m_is_some) {
            m_some.~T();
            m_is_some = false;
        }
    }

    // rusty methods
    constexpr bool is_some() const { return m_is_some; }

    template<class F>
    constexpr bool is_some_and(F&& f) &
        requires std::predicate<F, T&>
    {
        return m_is_some ? std::invoke(std::forward<F>(f), m_some) : false;
    }

    template<class F>
    constexpr bool is_some_and(F&& f) const&
        requires std::predicate<F, const T&>
    {
        return m_is_some ? std::invoke(std::forward<F>(f), m_some) : false;
    }

    template<class F>
    constexpr bool is_some_and(F&& f) &&
        requires std::predicate<F, T>
    {
        return m_is_some ? std::invoke(std::forward<F>(f), std::move(m_some)) : false;
    }

    template<class F>
    constexpr bool is_some_and(F&& f) const&&
        requires std::predicate<F, const T>
    {
        return m_is_some ? std::invoke(std::forward<F>(f), std::move(m_some)) : false;
    }

    constexpr bool is_none() const { return !m_is_some; }

    template<class F>
    constexpr bool is_none_or(F&& f) &
        requires std::predicate<F, T&>
    {
        return (!m_is_some) ? true : std::invoke(std::forward<F>(f), m_some);
    }

    template<class F>
    constexpr bool is_none_or(F&& f) const&
        requires std::predicate<F, const T&>
    {
        return (!m_is_some) ? true : std::invoke(std::forward<F>(f), m_some);
    }

    template<class F>
    constexpr bool is_none_or(F&& f) &&
        requires std::predicate<F, T>
    {
        return (!m_is_some) ? true : std::invoke(std::forward<F>(f), std::move(m_some));
    }

    template<class F>
    constexpr bool is_none_or(F&& f) const&&
        requires std::predicate<F, const T>
    {
        return (!m_is_some) ? true : std::invoke(std::forward<F>(f), std::move(m_some));
    }

    constexpr T& unwrap() & { return value(); }
    constexpr const T& unwrap() const & { return value(); }
    constexpr T&& unwrap() && { return value(); }
    constexpr const T&& unwrap() const && { return value(); }

    constexpr T& unwrap_unchecked() & { return m_some; }
    constexpr const T& unwrap_unchecked() const & { return m_some; }
    constexpr T&& unwrap_unchecked() && { return std::move(m_some); }
    constexpr const T&& unwrap_unchecked() const && { return std::move(m_some); }
}; // class optional
} // namespace my

namespace std {

template <class T>
struct formatter<my::optional<T>> {
    constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const my::optional<T>& p, format_context& ctx) const {
        if (p.has_value()) {
            return format_to(ctx.out(), "Some({})", p.unwrap_unchecked());
        } else {
            return format_to(ctx.out(), "None");
        }
    }
};


} // namespace std
