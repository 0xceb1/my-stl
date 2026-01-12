#pragma once
#include <cstddef>
#include <type_traits>
#include <concepts>
#include <utility>
#include "utility.hpp"

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
    constexpr allocator(const allocator&) noexcept : allocator() {}
    template<class U>
    constexpr allocator(const allocator<U>&) noexcept : allocator() {}
    // destructor
    constexpr ~allocator() = default;

    constexpr T* allocate(size_type n) {
        return static_cast<T*>(::operator new(n * sizeof(T)));
    }

    constexpr std::allocation_result<T*, size_type>
    allocate_at_least(size_type n) {
        return { allocate(n), n };
    }
    
    constexpr void deallocate(T* p) {
        ::operator delete(p);
    }

    constexpr void deallocate(T* p, size_type n) {
        ::operator delete(p, n * sizeof(T));
    }
}; // class allocator

template<class T1, class T2>
constexpr bool operator==( const allocator<T1>&, const allocator<T2>&) noexcept {
    return true;
}
// impl allocator

template<class T> struct default_delete {
    constexpr default_delete() noexcept = default;

    template<class U>
        requires std::convertible_to<U*, T*>
    constexpr default_delete(const default_delete<U>) noexcept {}

    constexpr void operator()(T* ptr) const {
        static_assert(0 < sizeof(T), "cannot delete an incomplete type");
        delete ptr;
    }
};

namespace {

struct zero_then_variadic_arg_t {
    explicit zero_then_variadic_arg_t() = default;
};

struct one_then_variadic_arg_t {
    explicit one_then_variadic_arg_t() = default;
};

template<class D, class T>
class compressed_pair {
public:
    D first;
    T second;

    template <class... Args2>
    constexpr explicit compressed_pair(zero_then_variadic_arg_t, Args2&&... args2)
        noexcept(std::is_nothrow_default_constructible_v<D> &&
                 std::is_nothrow_constructible_v<T, Args2...>)
    : first{}, second{std::forward<Args2>(args2)...} {}
    
    template <class Arg1, class... Args2>
    constexpr compressed_pair(one_then_variadic_arg_t, Arg1&& arg1, Args2&&... args2)
        noexcept(std::is_nothrow_constructible_v<D, Arg1> &&
                 std::is_nothrow_constructible_v<T, Args2...>)
    : first{std::forward<Arg1>(arg1)}, second{std::forward<Args2>(args2)...} {}

    constexpr D& get_first() noexcept { return first; }
    constexpr const D& get_first() const noexcept { return first; }
    constexpr T& get_second() noexcept { return second; }
    constexpr const T& get_second() const noexcept { return second; }
};

template<class D, class T>
    requires std::is_empty_v<D> && (!std::is_final_v<D>)
class compressed_pair<D, T> : private D {
public:
    T second;

    template <class... Args2>
    constexpr explicit compressed_pair(zero_then_variadic_arg_t, Args2&&... args2)
        noexcept(std::is_nothrow_default_constructible_v<D> &&
                 std::is_nothrow_constructible_v<T, Args2...>)
    : D{}, second{std::forward<Args2>(args2)...} {}
    
    template <class Arg1, class... Args2>
    constexpr compressed_pair(one_then_variadic_arg_t, Arg1&& arg1, Args2&&... args2)
        noexcept(std::is_nothrow_constructible_v<D, Arg1> &&
                 std::is_nothrow_constructible_v<T, Args2...>)
    : D{std::forward<Arg1>(arg1)}, second{std::forward<Args2>(args2)...} {}

    constexpr D& get_first() noexcept { return *this; }
    constexpr const D& get_first() const noexcept { return *this; }
    constexpr T& get_second() noexcept { return second; }
    constexpr const T& get_second() const noexcept { return second; }
};

template<class D>
concept unique_ptr_enable_default_t =
    !std::is_pointer_v<D> && std::default_initializable<D>;

} // anonymous namespace

template<class T, class D = default_delete<T>>
class unique_ptr {
public:
    using pointer      = T*;
    using element_type = T;
    using deleter_type = D;
private:
    compressed_pair<D, T*> m_pair;
    using A = std::remove_reference_t<D>;
public:

    // Constructors
    constexpr unique_ptr() noexcept
        requires unique_ptr_enable_default_t<D>
    : m_pair{zero_then_variadic_arg_t()} {}

    constexpr unique_ptr(std::nullptr_t) noexcept
        requires unique_ptr_enable_default_t<D>
    : unique_ptr() {}

    constexpr unique_ptr(pointer p) noexcept
        requires unique_ptr_enable_default_t<D>
    : m_pair{zero_then_variadic_arg_t(), p} {}

    // TODO: why we need these specifications:
    constexpr unique_ptr(pointer p, const A& d) noexcept
         requires (std::is_constructible_v<D, decltype(d)>) &&
                  (std::constructible_from<D, const A&>) && 
                  (!std::is_reference_v<D> ? std::is_nothrow_copy_constructible_v<D> : true)
    : m_pair{one_then_variadic_arg_t(), std::forward<decltype(d)>(d), p} {}

    constexpr unique_ptr(pointer p, A&& d) noexcept
         requires (std::is_constructible_v<D, decltype(d)>) &&
                  (std::constructible_from<D, A&&>) &&
                  (!std::is_reference_v<D>) &&
                  (std::is_nothrow_move_constructible_v<D>)
    : m_pair{one_then_variadic_arg_t(), std::forward<decltype(d)>(d), p} {}

    // move constructor
    constexpr unique_ptr(unique_ptr&& u) noexcept
        requires std::is_move_constructible_v<D>
    : m_pair{one_then_variadic_arg_t(),
             std::forward<D>(u.get_deleter()),
             u.release()} {}
    
    template<class U, class E>
    constexpr unique_ptr(unique_ptr<U, E>&& u) noexcept
        requires (std::is_convertible_v<typename unique_ptr<U, E>::pointer, pointer>) &&
                 (!std::is_array_v<U>) &&
                 ((std::is_reference_v<D> && std::same_as<E, D>) ||
                 (!std::is_reference_v<D> && std::convertible_to<E, D>))
    : m_pair{one_then_variadic_arg_t(),
             std::forward<E>(u.get_deleter()),
             u.release()} {}

    unique_ptr& operator=(unique_ptr&& r) noexcept
        requires std::is_move_assignable_v<D>
    {
        reset(r.release());
        get_deleter() = std::forward<D>(r.get_deleter());
        return *this;
    }

    template<class U, class E>
    unique_ptr& operator=(unique_ptr<U, E>&& r) noexcept
        requires (std::is_convertible_v<typename unique_ptr<U, E>::pointer, pointer>) &&
                 (!std::is_array_v<U>) &&
                 (std::is_assignable_v<D&, E&&>)
    {
        reset(r.release());
        get_deleter() = std::forward<E>(r.get_deleter());
        return *this;
    }

    unique_ptr& operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    // disable copy ctor & copy assign operator
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;

    // dtor
    constexpr ~unique_ptr() {
        if (get() != nullptr) {
            get_deleter()(get());
        }
    }

    // Modifiers
    constexpr pointer release() noexcept {
        return std::exchange(m_pair.get_second(), nullptr);
    }

    void reset(pointer ptr = pointer()) noexcept {
        auto old_ptr = std::exchange(m_pair.get_second(), ptr);
        if (old_ptr) {
           get_deleter()(old_ptr);
        }
    }

    void swap(unique_ptr& other) noexcept {
        if (*this != other) {
            std::swap(m_pair, other.m_pair);
        }
    }

    // Observers
    constexpr pointer get() const noexcept {
        return m_pair.get_second();
    }

    constexpr D& get_deleter() noexcept {
        return m_pair.get_first();
    }

    constexpr const D& get_deleter() const noexcept {
        return m_pair.get_first();
    }

    constexpr explicit operator bool() const noexcept {
        return get() != nullptr;
    }

    constexpr T& operator*() const noexcept {
        return *get();
    }

    constexpr pointer operator->() const noexcept {
        return get();
    }
}; // class unique_ptr

// Non-member functions for unique_ptr
template<class T, class... Args>
constexpr unique_ptr<T> make_unique(Args&&... args)
    requires (!std::is_array_v<T>)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template< class T >
constexpr unique_ptr<T> make_unique_for_overwrite()
    requires (!std::is_array_v<T>)
{
    return unique_ptr<T>(new T);
}

template<class T1, class D1, class T2, class D2>
constexpr bool operator==(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y) {
    return x.get() == y.get();
}

template<class T1, class D1, class T2, class D2>
    requires std::three_way_comparable_with<typename unique_ptr<T1, D1>::pointer,
                                            typename unique_ptr<T2, D2>::pointer>
constexpr std::compare_three_way_result_t<typename unique_ptr<T1, D1>::pointer,
                                          typename unique_ptr<T2, D2>::pointer>
    operator<=>(const unique_ptr<T1, D1>& x, const unique_ptr<T2, D2>& y)
{
    return std::compare_three_way{}(x.get(), y.get());
}

template<class T, class D>
constexpr bool operator==(const unique_ptr<T, D>& x, std::nullptr_t) noexcept {
    return !x;
}

template<class T, class D>
    requires std::three_way_comparable<typename unique_ptr<T, D>::pointer>
constexpr std::compare_three_way_result_t<typename unique_ptr<T, D>::pointer>
    operator<=>(const unique_ptr<T, D>& x, std::nullptr_t)
{
    return std::compare_three_way{}(x.get(), static_cast<typename unique_ptr<T, D>::pointer>(nullptr));
}

} // namespace my
