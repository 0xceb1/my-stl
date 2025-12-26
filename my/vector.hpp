#pragma once
#include <algorithm>
#include <iterator>
#include <memory>
#include <print>
#include "memory.hpp"
#include "utility.hpp"

#define DEFAULT_INIT_SIZE 0
#define MAX_SIZE (1024 * 1024 * 1024)

namespace my {
template<
    class T,
    class Allocator = allocator<T>
> class vector {
public:
    using value_type             = T;
    using allocator_type         = Allocator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;
    using reference              = value_type&;
    using const_reference        = const value_type&;
    using pointer                = value_type*;
    using const_pointer          = const value_type*;
    using iterator               = value_type*;
    using const_iterator         = const value_type*;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
private:
    pointer m_st;
    size_type m_sz;
    size_type m_cap;
    allocator_type m_alloc;
public:
    // For debug purpose only
    void show() {
        std::print("[");
        for (size_type i = 0; i < m_sz; ++i) {
            std::print("{}, ", m_st[i]);
        }
        std::println("]");
        std::println("m_st: {}, m_sz: {}, m_cap: {}", static_cast<void*>(begin()), size(), capacity());
    }
    
    // constructors
    constexpr vector() : vector(Allocator()) {}
    
    constexpr explicit vector(const Allocator& alloc) noexcept :
        m_st{nullptr}, m_sz{DEFAULT_INIT_SIZE}, m_cap{DEFAULT_INIT_SIZE}, m_alloc{alloc}
    {
        m_st = m_alloc.allocate(DEFAULT_INIT_SIZE);
    }
    
    explicit vector(size_type count, const Allocator& alloc = Allocator()) :
        m_st{nullptr}, m_sz{count}, m_cap{count}, m_alloc(alloc)
    {
        m_st = m_alloc.allocate(count);
    }

    constexpr vector(size_type count, const T& value,
        const Allocator& alloc = Allocator())
    {
        m_alloc = alloc;
        m_st = m_alloc.allocate(count);
        m_sz = m_cap = count;
        std::uninitialized_fill_n(m_st, count, value);
    }

    template<class InputIt>
    constexpr vector(InputIt first, InputIt last,
            const Allocator& alloc = Allocator())
    {
        size_type count = std::distance(first, last);
        m_alloc = alloc;
        m_st = m_alloc.allocate(count);
        m_sz = m_cap = count;
        std::uninitialized_copy(first, last, m_st);
    }

    constexpr vector(const vector& other) {
        size_type count = other.m_sz;
        m_alloc = other.get_allocator();
        m_st = m_alloc.allocate(count);
        m_sz = m_cap = count;
        std::uninitialized_copy(other.cbegin(), other.cend(), m_st);
    }

    constexpr vector(vector&& other) noexcept :
        m_st{my::exchange(other.m_st, nullptr)},
        m_sz{my::exchange(other.m_sz, 0)},
        m_cap{my::exchange(other.m_cap, 0)},
        m_alloc{std::move(other.m_alloc)} {}

    vector(std::initializer_list<T> init,
        const Allocator& alloc = Allocator()) : vector{init.begin(), init.end(), alloc} {}

    constexpr allocator_type get_allocator() const noexcept {
        return m_alloc;
    }

    // destructor
    constexpr ~vector() {
        std::destroy(begin(), end());
        m_alloc.deallocate(my::exchange(m_st, nullptr), m_cap);
        m_sz = m_cap = 0;
    }

    // element access
    constexpr pointer data() noexcept {
        return m_st;
    }

    constexpr const_pointer data() const noexcept {
        return m_st;
    }
    // iterators
    constexpr iterator begin() noexcept {
        return m_st;
    }

    constexpr const_iterator begin() const noexcept {
        return m_st;
    }

    constexpr const_iterator cbegin() const noexcept {
        return m_st;
    }

    constexpr iterator end() noexcept {
        return m_st + m_sz;
    }

    constexpr const_iterator end() const noexcept {
        return m_st + m_sz;
    }

    constexpr const_iterator cend() const noexcept {
        return m_st + m_sz;
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator(end());
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator(begin());
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    // capacity
    constexpr bool empty() const noexcept {
        return m_sz == 0;
    }

    constexpr size_type size() const noexcept {
        return m_sz;
    }

    constexpr size_type max_size() const noexcept {
        return MAX_SIZE;
    }

    constexpr size_type capacity() const noexcept {
        return m_cap;
    }

}; // class vector
} // namespace my
