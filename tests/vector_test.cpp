#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <ranges>
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
namespace stdr = std::ranges;

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

    SECTION("copy constructed") {
        VecStr vs1(n, "bar");
        VecStr vs2 { vs1 };
        REQUIRE(vs2.size() == vs1.size());
        REQUIRE(vs2[i] == vs1[i]);
    }

    SECTION("move constructed") {
        VecStr vs1(n, "bar");
        VecStr vs2 { std::move(vs1) };
        REQUIRE(vs1.empty());
        REQUIRE(vs2.size() == n);
        REQUIRE(vs2[i] == "bar");
    }

    SECTION("constructed from ilist") {
        VecInt vi1{10, 2};
        VecInt vi2(10, 2);
        REQUIRE(vi1.size() == 2);
        REQUIRE(vi1[0] == 10);
        REQUIRE(vi1[1] == 2);
        REQUIRE(vi2.size() == 10);
    }
}

TEST_CASE("my::vector can be inserted", "[my::vector]") {

    constexpr size_t n = 1024;
    VecStr empty{};
    VecStr vs1(n, "foo");
    std::vector<string> std_vs1{std::from_range, vs1};
    VecStr vs2(n, "bar");
    std::vector<string> std_vs2{std::from_range, vs2};
    vs1.reserve(4 * n);
    vs2.shrink_to_fit();

    auto offset = GENERATE(take(5, random(size_t(0), n)));
    auto n_insert = GENERATE(take(5, random(size_t(0), n)));

    SECTION("inserted a single element") {
        SECTION("empty vec") {
            empty.insert(empty.begin(), "baz");
            REQUIRE(empty.front() == "baz");
            REQUIRE(empty.size() == 1);
        }
        SECTION("capacity is enough") {
            auto init_pos = vs1.cbegin();
            vs1.insert(vs1.begin() + offset, "baz");
            std_vs1.insert(std_vs1.begin() + offset, "baz");
            REQUIRE(stdr::equal(vs1, std_vs1));

            vs1.insert(vs1.begin(), "baz");
            std_vs1.insert(std_vs1.begin(), "baz");
            REQUIRE(stdr::equal(vs1, std_vs1));

            vs1.insert(vs1.end(), "baz");
            std_vs1.insert(std_vs1.end(), "baz");
            REQUIRE(stdr::equal(vs1, std_vs1));

            REQUIRE(vs1.capacity() == 4 * n);
            REQUIRE(vs1.cbegin() == init_pos);
        }
        SECTION("capacity is not enough") {
            auto init_pos = vs2.cbegin();
            vs2.insert(vs2.begin() + offset, "baz");
            std_vs2.insert(std_vs2.begin() + offset, "baz");
            REQUIRE(stdr::equal(vs2, std_vs2));

            vs2.insert(vs2.begin(), "baz");
            std_vs2.insert(std_vs2.begin(), "baz");
            REQUIRE(stdr::equal(vs2, std_vs2));

            vs2.insert(vs2.end(), "baz");
            std_vs2.insert(std_vs2.end(), "baz");
            REQUIRE(stdr::equal(vs2, std_vs2));

            REQUIRE(vs2.capacity() > n);
            REQUIRE_FALSE(vs2.cbegin() ==  init_pos);
        }
    }
    SECTION("insert n elements") {
        SECTION("insert 0 elements") {
            auto vs1_copy = vs1;
            vs1.insert(vs1.begin() + offset, 0, "who cares");
            REQUIRE(vs1 == vs1_copy);
        }
        SECTION("capacity is enough") {
            CAPTURE(offset, n_insert);
            auto init_pos = vs1.cbegin();
            vs1.insert(vs1.begin() + offset, n_insert, "baz");
            std_vs1.insert(std_vs1.begin() + offset, n_insert, "baz");
            REQUIRE(stdr::equal(vs1, std_vs1));

            vs1.insert(vs1.begin(), n_insert, "baz");
            std_vs1.insert(std_vs1.begin(), n_insert, "baz");
            REQUIRE(stdr::equal(vs1, std_vs1));

            vs1.insert(vs1.end(), n_insert, "baz");
            std_vs1.insert(std_vs1.end(), n_insert, "baz");
            REQUIRE(stdr::equal(vs1, std_vs1));

            REQUIRE(vs1.capacity() == 4 * n);
            REQUIRE(vs1.cbegin() == init_pos);
        }
        SECTION("capacity is not enough") {
            CAPTURE(offset, n_insert);
            auto init_pos = vs2.cbegin();
            vs2.insert(vs2.begin() + offset, n_insert, "baz");
            std_vs2.insert(std_vs2.begin() + offset, n_insert, "baz");
            REQUIRE(stdr::equal(vs2, std_vs2));

            vs2.insert(vs2.begin(), n_insert, "baz");
            std_vs2.insert(std_vs2.begin(), n_insert, "baz");
            REQUIRE(stdr::equal(vs2, std_vs2));

            vs2.insert(vs2.end(), n_insert, "baz");
            std_vs2.insert(std_vs2.end(), n_insert, "baz");
            REQUIRE(stdr::equal(vs2, std_vs2));

            REQUIRE(vs2.capacity() > n);
            REQUIRE_FALSE(vs2.cbegin() == init_pos);
        }
    }
    SECTION("insert from iterator") {
        list<string> inserter(n_insert, "baz");
        SECTION("insert from empty range") {
            auto vs1_copy = vs1;
            list<string> empty_inserter{};
            vs1.insert(vs1.begin() + offset, empty_inserter.begin(), empty_inserter.end());
            REQUIRE(vs1 == vs1_copy);
        }
        SECTION("capacity is enough") {
            CAPTURE(offset, n_insert);
            auto init_pos = vs1.cbegin();
            vs1.insert(vs1.begin() + offset, inserter.begin(), inserter.end());
            std_vs1.insert(std_vs1.begin() + offset, inserter.begin(), inserter.end());
            REQUIRE(stdr::equal(vs1, std_vs1));

            vs1.insert(vs1.begin(), inserter.begin(), inserter.end());
            std_vs1.insert(std_vs1.begin(), inserter.begin(), inserter.end());
            REQUIRE(stdr::equal(vs1, std_vs1));

            vs1.insert(vs1.end(), inserter.begin(), inserter.end());
            std_vs1.insert(std_vs1.end(), inserter.begin(), inserter.end());
            REQUIRE(stdr::equal(vs1, std_vs1));

            REQUIRE(vs1.capacity() == 4 * n);
            REQUIRE(vs1.cbegin() == init_pos);
        }
        SECTION("capacity is not enough") {
            CAPTURE(offset, n_insert);
            auto init_pos = vs2.cbegin();
            vs2.insert(vs2.begin() + offset, inserter.begin(), inserter.end());
            std_vs2.insert(std_vs2.begin() + offset, inserter.begin(), inserter.end());
            REQUIRE(stdr::equal(vs2, std_vs2));

            vs2.insert(vs2.begin(), inserter.begin(), inserter.end());
            std_vs2.insert(std_vs2.begin(), inserter.begin(), inserter.end());
            REQUIRE(stdr::equal(vs2, std_vs2));

            vs2.insert(vs2.end(), inserter.begin(), inserter.end());
            std_vs2.insert(std_vs2.end(), inserter.begin(), inserter.end());
            REQUIRE(stdr::equal(vs2, std_vs2));

            REQUIRE(vs2.capacity() > n);
            REQUIRE_FALSE(vs2.cbegin() == init_pos);
        }
    }
}

