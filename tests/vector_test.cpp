#include <cstddef>
#include <string>
#include <vector>
#include <list>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include "my/vector.hpp"

using my::vector;
using std::list;
using std::string;
using VecStr = vector<string>;
using VecInt = vector<int>;

TEST_CASE("my::vector can be constructed", "[my::vector]") {

    constexpr size_t n = 1024;
    auto i = GENERATE(take(100, random(size_t(0), n - 1)));

    SECTION("default constructed") {
        VecStr empty{};
        REQUIRE(empty.size() == 0);
        REQUIRE(empty.capacity() >= 0);
        REQUIRE(empty.begin() == empty.end());
    }

    SECTION("constructed from count") {
        VecStr vs(n);
        REQUIRE(vs.size() == n);
        REQUIRE(vs.capacity() >= n);
        REQUIRE(vs[i] == "");
    }

    SECTION("constructed from count and value") {
        VecStr vs(n, "foo");
        REQUIRE(vs.size() == n);
        REQUIRE(vs.capacity() >= n);
        REQUIRE(vs[i] == "foo");
    }

    SECTION("constructed from two iterators") {
        list<string> ls(n, "foo");
        VecStr vs(ls.begin(), ls.end());
        REQUIRE(vs.size() == n);
        REQUIRE(vs.capacity() >= n);
        REQUIRE(vs[i] == "foo");
    }

//     SECTION("move constructed") {
//         // TODO: forward move
//         MaybeStr moved_some{ std::move(some) };
//         REQUIRE(some); // some is still true
//         REQUIRE(some.value() == "");
//         REQUIRE(moved_some.value() == "hello");
//     }

//     SECTION("construced from content in place & ilist") {
//         MaybeVec some_vec(std::in_place, {1, 2, 3, 4, 5});
//         REQUIRE(some_vec.value().size() == 5);
//     }
}
