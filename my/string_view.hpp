#pragma once
#include <concepts>
#include <cstddef>
#include <format>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <iterator>

namespace my {

template< class CharT, class Traits = std::char_traits<CharT>>
    requires (!std::is_array_v<CharT>) &&
             (std::is_trivially_copyable_v<CharT>) &&
             (std::is_trivially_default_constructible_v<CharT>) &&
             (std::is_standard_layout<CharT>) &&
             (std::is_same_v<CharT, typename Traits::char_type>)
class basic_string_view {
public:
    using traits_type            = Traits;
    using value_type             = CharT;
    using pointer                = CharT*;
    using const_pointer          = const CharT*;
    using reference              = CharT&;
    using const_reference        = const CharT&;
    using const_iterator         = const CharT*;
    using iterator               = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator	     = const_reverse_iterator;
    using size_type              = std::size_t;
    using difference_type        = std::ptrdiff_t;

    // constants
    static constexpr size_type npos = size_type(-1);
private:
    const CharT* m_data;
    std::size_t  m_size;
public:
    constexpr basic_string_view() noexcept
    : m_data{nullptr}, m_size{0} {}

    constexpr basic_string_view(const basic_string_view& other) noexcept = default;
    constexpr basic_string_view(const CharT* s, size_type count)
    : m_data{s}, m_size{count} {}

    constexpr basic_string_view(const CharT* s)
    : m_data{s}, m_size{Traits::length(s)} {}

    template<std::contiguous_iterator It, std::sized_sentinel_for<It> End>
        requires (std::same_as<CharT, std::iter_value_t<It>>) &&
                 (!std::convertible_to<End, size_type>)
    constexpr basic_string_view(It first, End last) 
    : m_data{std::to_address(first)}, m_size{last - first} {}

    basic_string_view(std::nullptr_t) = delete;

    constexpr basic_string_view& operator=(const basic_string_view& view) noexcept = default;

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept {
      return m_data;
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept {
      return m_data + m_size;
    }

    [[nodiscard]]
    constexpr const_iterator cbegin() const noexcept {
      return m_data;
    }

    [[nodiscard]]
    constexpr const_iterator cend() const noexcept {
      return m_data + m_size;
    }

    [[nodiscard]]
    constexpr const_reverse_iterator rbegin() const noexcept {
      return std::const_reverse_iterator(end());
    }

    [[nodiscard]]
    constexpr const_reverse_iterator rend() const noexcept {
      return std::const_reverse_iterator(begin());
    }

    [[nodiscard]]
    constexpr const_reverse_iterator crbegin() const noexcept {
      return std::const_reverse_iterator(end());
    }

    [[nodiscard]]
    constexpr const_reverse_iterator crend() const noexcept {
      return std::const_reverse_iterator(begin());
    }

    [[nodiscard]]
    constexpr const_reference operator[](size_type pos) const {
        return m_data[pos];
    }

    [[nodiscard]]
    constexpr const_reference at(size_type pos) const {
        if (pos >= m_size) {
            throw std::out_of_range(
                std::format("basic_string_view::at out of range: {} >= {}",
                            pos, m_size)
            );
        }
        return m_data[pos];
    }

    [[nodiscard]]
    constexpr const_reference front() const {
        return m_data[0];
    }

    [[nodiscard]]
    constexpr const_reference back() const {
        return m_data[m_size - 1];
    }

    [[nodiscard]]
    constexpr const_pointer data() const noexcept {
        return m_data
    }

    [[nodiscard]]
    constexpr size_type size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    constexpr size_type length() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    constexpr size_type max_size() const noexcept {
        return (npos - sizeof(size_type) - sizeof(void*))
                / sizeof(value_type)
                / 2;
    }

    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return m_size == 0;
    }

}; // class basic_string_view

using string_view = basic_string_view<char>;
} // namespace my
