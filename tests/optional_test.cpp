#include <cctype>
#include <format>
#include <optional>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include "my/optional.hpp"

using my::optional;
using std::string;
using std::vector;
using MaybeStr = optional<string>;
using MaybeCstr = optional<const char *>;
using MaybeVec = optional<vector<int>>;

TEST_CASE("my::optional can be constructed", "[my::optional]") {
    MaybeStr none {};
    MaybeStr some { "hello" };
    MaybeCstr some_cstr { "world" };
    optional<MaybeStr> some_none = MaybeStr {};

    SECTION("default constructed") {
        REQUIRE_FALSE(none);
    }

    SECTION("constructed from content") {
        REQUIRE(some.value() == "hello");
        REQUIRE(some_none);
        REQUIRE_FALSE(some_none.value());
    }

    SECTION("copy constructed") {
        MaybeStr copied_some{ some };
        MaybeStr forward_copied{ some_cstr };
        REQUIRE(some.value() == "hello");
        REQUIRE(copied_some.value() == "hello");
        REQUIRE(std::strcmp(some_cstr.value(), "world") == 0);
        REQUIRE(forward_copied.value() == "world");
    }

    SECTION("move constructed") {
        // TODO: forward move
        MaybeStr moved_some{ std::move(some) };
        REQUIRE(some); // some is still true
        REQUIRE(some.value() == "");
        REQUIRE(moved_some.value() == "hello");
    }

    SECTION("construced from content in place & ilist") {
        MaybeVec some_vec(std::in_place, {1, 2, 3, 4, 5});
        REQUIRE(some_vec.value().size() == 5);
    }
}

TEST_CASE("my::optioanl can work like a pointer", "[my::optional]")  {
    // UB if has_value() is false
    MaybeStr some{ "hello" };
    REQUIRE(some->size() == 5);
    REQUIRE(*some == "hello");
    *some = "world";
    REQUIRE(*some == "world");
}

TEST_CASE("my::optioanl can invoke another function with value_or", "[my::optional]") {
    MaybeStr some{};
    string str{ "world" };
    REQUIRE(some.value_or(str) == "world");
    some.emplace("hello");
    REQUIRE(some.value_or(str) == "hello");
}

TEST_CASE("my::optional is functional", "[my::optional]") {
    MaybeStr none{};
    MaybeStr some{ "hello" };
    auto to_some_world = [](string) -> MaybeStr { return "world"; };
    auto to_world = [](string) -> string { return "world"; };
    auto get_world = [] -> string { return "world"; };

    SECTION("and_then: m a -> (a -> m b) -> m b") {
        REQUIRE_FALSE(none.and_then(to_some_world));
        REQUIRE(some.and_then(to_some_world).value() == "world");
    }
    SECTION("transform: m a -> (a -> b) -> m b") {
        REQUIRE_FALSE(none.transform(to_world));
        REQUIRE(some.transform(to_world).value() == "world");
    }
    SECTION("or_else: m a -> a -> m a") {
        REQUIRE(some.or_else(get_world).value() == "hello");
        REQUIRE(none.or_else(get_world).value() == "world");
    }
}

TEST_CASE("my::optional is rusty", "[my::optional]") {
    MaybeStr none{};
    MaybeStr some{ "hello" };

    SECTION("unwrap will throw, unwrap_unchecked is UB") {
        REQUIRE_THROWS(none.unwrap());
        REQUIRE_NOTHROW(none.unwrap_unchecked());
    }

    SECTION("my::optional can be chained with predicates") {
        auto is_all_lowercase = [](string s) -> bool {
            return std::find_if(s.begin(), s.end(), [](char c) { return !std::islower(static_cast<unsigned char>(c)); }) == s.end();
        };
        MaybeStr Some { "Hello" };
        REQUIRE_FALSE(none.is_some_and(is_all_lowercase));
        REQUIRE(some.is_some_and(is_all_lowercase));
        REQUIRE_FALSE(Some.is_some_and(is_all_lowercase));
        REQUIRE(none.is_none_or(is_all_lowercase));
        REQUIRE(some.is_none_or(is_all_lowercase));
    }
}

TEST_CASE("my::optional can be formatted & printed", "[my::optional]") {
    MaybeStr none{};
    MaybeStr some{ "hello" };
    optional<int> some_int{42};
    MaybeVec some_vec{ std::in_place, {1, 2, 3, 4, 5}};

    REQUIRE(std::format("{}", none) == "None");
    REQUIRE(std::format("{}", some) == std::format("Some({})", some.value()));
    REQUIRE(std::format("{}", some_int) == std::format("Some({})", some_int.value()));
    REQUIRE(std::format("{}", some_vec) == std::format("Some({})", some_vec.value()));
}
