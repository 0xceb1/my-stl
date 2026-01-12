#pragma once
#include <cstddef>
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

}; // class basic_string_view

using string_view = basic_string_view<char>;
} // namespace my
