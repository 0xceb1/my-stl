#pragma once
#include <concepts>
#include <cstddef>
#include <cstring>
#include <format>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <iterator>
#include <ranges>

namespace stdv = std::ranges::views;

namespace my {

template< class CharT, class Traits = std::char_traits<CharT>>
    requires (!std::is_array_v<CharT>) &&
             (std::is_trivially_copyable_v<CharT>) &&
             (std::is_trivially_default_constructible_v<CharT>) &&
             (std::is_standard_layout_v<CharT>) &&
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
      return const_reverse_iterator(end());
    }

    [[nodiscard]]
    constexpr const_reverse_iterator rend() const noexcept {
      return const_reverse_iterator(begin());
    }

    [[nodiscard]]
    constexpr const_reverse_iterator crbegin() const noexcept {
      return const_reverse_iterator(end());
    }

    [[nodiscard]]
    constexpr const_reverse_iterator crend() const noexcept {
      return const_reverse_iterator(begin());
    }

    [[nodiscard]]
    constexpr const_reference operator[](size_type pos) const {
        return m_data[pos];
    }

    [[nodiscard]]
    constexpr const_reference at(size_type pos) const {
        if (pos >= m_size) {
            throw std::out_of_range(
                std::format("basic_string_view::at out of range:"
                            "pos (which is {}) >= this->size() (which is {})",
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
        return m_data;
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

    constexpr void remove_prefix(size_type n) {
        m_data += n;
        m_size -= n;
    }

    constexpr void remove_suffix(size_type n) {
        m_size -= n;
    }

    constexpr void swap(basic_string_view& v) noexcept {
        auto tmp = *this;
        *this = v;
        v = tmp;
    }

    constexpr size_type copy(CharT* dest, size_type count, size_type pos = 0) const {
        if (pos >= m_size) {
            throw std::out_of_range(
                std::format("basic_string_view::copy out of range:"
                            "pos (which is {}) >= this->size() (which is {})",
                            pos, m_size)
            );
        }
        size_type rcount = std::min<size_type>(count, m_size - pos);
        // should use char_traits::copy
        // return Traits::copy(dest, data() + pos, rcount);
        std::memcpy(dest, data() + pos, rcount * sizeof(CharT));
        return rcount;
    }

    constexpr basic_string_view substr(size_type pos = 0,
                                       size_type count = npos) const
    {
        if (pos >= m_size) {
            throw std::out_of_range(
                std::format("basic_string_view::substr out of range:"
                            "pos (which is {}) >= this->size() (which is {})",
                            pos, m_size)
            );
        }
        size_type rcount = std::min<size_type>(count, m_size - pos);
        return basic_string_view{ m_data + pos, rcount };
    }

    constexpr int compare(basic_string_view v) const noexcept {
        size_type rlen = std::min<size_type>(m_size, v.m_size);
        int res = Traits::compare(m_data, v.m_data, rlen);
        if (res == 0)
            return static_cast<int>(m_size - v.m_size);
        return res;
    }

    constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const {
        return substr(pos1, count1).compare(v);
    }

    constexpr int compare(size_type pos1, size_type count1, basic_string_view v,
                          size_type pos2, size_type count2) const
    {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
    }

    constexpr int compare(const CharT* s) const {
        return compare(basic_string_view {s});
    }

    constexpr int compare(size_type pos1, size_type count1, const CharT* s) const {
        return substr(pos1, count1).compare(basic_string_view {s});
    }

    constexpr int compare(size_type pos1, size_type count1,
                          const CharT* s, size_type count2) const
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    constexpr bool starts_with(basic_string_view sv) const noexcept {
        return m_size >= sv.m_size &&
            compare(0, sv.m_size, sv) == 0;
    }

    constexpr bool starts_with(CharT ch) const noexcept {
        return !empty() && Traits::eq(front(), ch);
    }

    constexpr bool starts_with(const CharT* s) const {
        return starts_with(basic_string_view(s));
    }

    constexpr bool ends_with(basic_string_view sv) const noexcept {
        return m_size >= sv.m_size &&
            compare(m_size - sv.m_size, sv.m_size, sv) == 0;
    }

    constexpr bool ends_with(CharT ch) const noexcept {
        return !empty() && Traits::eq(back(), ch);
    }

    constexpr bool ends_with(const CharT* s) const {
        return ends_with(basic_string_view(s));
    }

    constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept {
        return find(v.data(), pos, v.size());
    }

    constexpr size_type find(CharT ch, size_type pos = 0) const noexcept {
        if (pos >= m_size) return npos;
        const CharT* result = Traits::find(m_data + pos, m_size - pos, ch);
        return result ? result - m_data : npos;
    }

    constexpr size_type find(const CharT* s, size_type pos, size_type count) const {
        if (count == 0) return pos <= m_size ? pos : npos;
        if (pos >= m_size || count > m_size - pos) return npos;

        const CharT elem0 = s[0];
        const CharT* first = m_data + pos;
        const CharT* const last = m_data + m_size;
        size_type len = m_size - pos;

        while (len >= count) {
            // find first char
            first = Traits::find(first, len - count + 1, elem0);
            if (!first)
                return npos;
            if (Traits::compare(first, s, count) == 0)
                return first - m_data;
            len = last - (++first);
        }
        return npos;
    }

    constexpr size_type find(const CharT* s, size_type pos = 0) const {
        return find(s, pos, Traits::length(s));
    }

    constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept {
        return rfind(v.data(), pos, v.size());
    }

    constexpr size_type rfind(CharT ch, size_type pos = npos) const noexcept {
        return rfind(std::addressof(ch), pos, 1);
    }

    constexpr size_type rfind(const CharT* s, size_type pos, size_type count) const {
        if (count == 0) return std::min(pos, m_size);
        if (count > m_size) return npos;

        pos = std::min<size_type>(m_size - count, pos);

        do {
            if (Traits::compare(m_data + pos, s, count) == 0)
                return pos;
        } while (pos-- > 0);
    
        return npos;
    }

    constexpr size_type rfind(const CharT* s, size_type pos = npos) const {
        return rfind(s, pos, Traits::length(s));
    }

    constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept {
        for (size_type i = pos; i < m_size; ++i) {
            if (Traits::find(v.m_data, v.m_size, m_data[i]))
                return i;
        }
        return npos;
    }

    constexpr size_type find_first_of(CharT ch, size_type pos = 0) const noexcept {
        return find(ch, pos);
    }

    constexpr size_type find_first_of(const CharT* s, size_type pos, size_type count) const {
        return find_first_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_first_of(const CharT* s, size_type pos = 0) const {
        return find_first_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept {
        if (m_size == 0 || v.m_size == 0) return npos;
        size_type last = std::min<size_type>(pos, m_size - 1);
        do {
            if (Traits::find(v.m_data, v.m_size, m_data[last]))
                return last;
        } while (last-- > 0);
        return npos;
    }

    constexpr size_type find_last_of(CharT ch, size_type pos = npos) const noexcept {
        return rfind(ch, pos);
    }

    constexpr size_type find_last_of(const CharT* s, size_type pos, size_type count) const {
        return find_last_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_last_of(const CharT* s, size_type pos = npos) const {
        return find_last_of(basic_string_view(s), pos);
    }

    constexpr size_type find_first_not_of(basic_string_view v, size_type pos = 0) const noexcept {
        for (size_type i = pos; i < m_size; ++i) {
            if (!Traits::find(v.m_data, v.m_size, m_data[i]))
                return i;
        }
        return npos;
    }

    constexpr size_type find_first_not_of(CharT ch, size_type pos = 0) const noexcept {
        return find_first_not_of(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find_first_not_of(const CharT* s, size_type pos, size_type count) const {
        return find_first_not_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_first_not_of(const CharT* s, size_type pos = 0) const {
        return find_first_not_of(basic_string_view(s), pos);
    }

    constexpr size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept {
        if (m_size == 0) return npos;
        size_type last = std::min<size_type>(pos, m_size - 1);
        do {
            if (!Traits::find(v.m_data, v.m_size, m_data[last]))
                return last;
        } while (last-- > 0);
        return npos;
    }

    constexpr size_type find_last_not_of(CharT ch, size_type pos = npos) const noexcept {
        return find_last_not_of(basic_string_view(std::addressof(ch), 1), pos);
    }

    constexpr size_type find_last_not_of(const CharT* s, size_type pos, size_type count) const {
        return find_last_not_of(basic_string_view(s, count), pos);
    }

    constexpr size_type find_last_not_of(const CharT* s, size_type pos = npos) const {
        return find_last_not_of(basic_string_view(s), pos);
    }

}; // class basic_string_view

template<class CharT, class Traits>
constexpr bool operator==(basic_string_view<CharT,Traits> lhs,
                          basic_string_view<CharT,Traits> rhs) noexcept {
    
}

using string_view = my::basic_string_view<char>;
} // namespace my
