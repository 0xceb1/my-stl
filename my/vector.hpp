#pragma once
#include <algorithm>
#include <iterator>
#include <memory>
#include <print>
#include <stdexcept>
#include "memory.hpp"
#include "utility.hpp"

#define DEFAULT_INIT_SIZE 0
#define MAX_SIZE (1024 * 1024 * 1024)
#define REALLOCATION_FACTOR 2

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
        m_alloc.deallocate(my::exchange(m_st, nullptr));
        m_sz = m_cap = 0;
    }

    // element access
    constexpr reference at(size_type pos) {
        if (pos >= m_sz) {
            throw std::out_of_range("Index out of range");
        } else {
            return m_st[pos];
        }
    }

    constexpr reference operator[](size_type pos) {
        return m_st[pos];
    }

    constexpr const_reference operator[](size_type pos) const {
        return m_st[pos];
    }

    constexpr reference front() {
        return m_st[0];
    }

    constexpr const_reference front() const {
        return m_st[0];
    }

    constexpr reference back() {
        return m_st[m_sz - 1];
    }

    constexpr const_reference back() const {
        return m_st[m_sz - 1];
    }

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

    void reserve(size_type new_cap) {
        if (new_cap > MAX_SIZE) {
            throw std::length_error("Try to allocate space larger than max_size()");
        } else if (new_cap > m_cap) {
            pointer new_data = m_alloc.allocate(new_cap);
            std::uninitialized_move(begin(), end(), new_data);
            std::destroy(begin(), end());
            m_alloc.deallocate(m_st);
            m_st = new_data;
            m_cap = new_cap;
        } else {
            return;
        }
    }

    constexpr void shrink_to_fit() {
        if (m_sz == m_cap) {
            return;
        } else if (m_sz == 0){
            std::destroy(begin(), end());
            m_alloc.deallocate(m_st);
            m_st = nullptr;
            m_cap = 0;
        } else {
            pointer new_data = m_alloc.allocate(m_sz);
            std::uninitialized_move(begin(), end(), new_data);
            std::destroy(begin(), end());
            m_alloc.deallocate(m_st);
            m_st = new_data;
            m_cap = m_sz;
        }
    }

    constexpr size_type capacity() const noexcept {
        return m_cap;
    }

    // modifiers
    constexpr void clear() noexcept {
        std::destroy(begin(), end());
        m_sz = 0;
    }

    constexpr void push_back(const_reference value) { emplace_back(value); }

    constexpr void push_back(value_type &&value) {
        emplace_back(std::move(value));
    }

    template<class... Args>
    constexpr reference emplace_back(Args&&... args) {
        if (m_sz == m_cap) {
            size_type new_cap = (m_cap == 0) ? 1 : REALLOCATION_FACTOR * m_cap;
            reserve(new_cap);
        }
        return *std::construct_at(m_st + (m_sz++), std::forward<Args>(args)...);
    }

    constexpr void pop_back() {
        if (empty()) {
            return;
        }
        std::destroy_at(m_st + m_sz - 1);
        m_sz--;
    }

    constexpr void swap(vector &other) noexcept {
        if (this != &other) {
            std::swap(m_st, other.m_st);
            std::swap(m_sz, other.m_sz);
            std::swap(m_cap, other.m_cap);
        }
    }
}; // class vector
} // namespace my
