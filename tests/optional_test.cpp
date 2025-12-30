#include <optional>
#include <vector>
#include <string>
#include <cstring>
#include <utility>
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
        REQUIRE(some);
        REQUIRE(some.value() == "");
        REQUIRE(moved_some.value() == "hello");
    }

    SECTION("construced from content in place & ilist") {
        MaybeVec some_vec(std::in_place, {1, 2, 3, 4, 5});
        REQUIRE(some_vec.value().size() == 5);
    }
}

