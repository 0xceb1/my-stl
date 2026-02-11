#include <algorithm>
#include <memory>
#include <print>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
#include <cassert>
#include "my/memory.hpp"
#include "my/utility.hpp"
#include "my/vector.hpp"
#include "my/optional.hpp"
#include "my/string_view.hpp"

using std::println;
namespace stdr = std::ranges;
namespace stdv = std::ranges::views;

void test_allocator(void);
void test_pair(void);
void test_vector(void);
void test_optional(void);

int main(void) {
    // test_allocator();
    // test_pair();
    // test_vector();
    // test_optional();
    constexpr my::string_view str {"123456789"};
    static_assert(str == "123456789");
    static_assert(str > "1234567");
}

void test_allocator(void) {
    using std::string;
    using std::vector;

    auto allocator = my::allocator<string>();
    std::size_t n = 10;
    string* p_st = allocator.allocate(n);

    for (size_t i = 0; i < n; ++i) {
        new(p_st + i) string{"urmom"};
    }


    for (size_t i = 0; i < n; ++i) {
        println("i={}, {}", i, p_st[i]);
    }

    std::destroy(p_st, p_st + n);
    allocator.deallocate(p_st, n);

    vector<string, my::allocator<string>> s_vec(10, "urmom");
    auto _ = std::for_each(s_vec.begin(), s_vec.end(), [](const string& s) { println("{}", s); });
}

void test_pair(void) {
    using std::string;
    using Vec = std::vector<int>;

    string s = "urmom";
    Vec v(5, 69);

    auto pair1 = my::pair<string, Vec>();
    pair1.show();

    auto pair2 = my::pair<string, Vec>(s, v);
    pair2.show();

    pair1 = pair2;
    pair1.show();

    pair1 = std::move(pair2);
    pair2.show();

    auto pair3 = my::make_pair(s, v);
    pair3.show();

    assert(pair1 == pair3);
    assert(pair1 != pair2);
    assert(pair1 <= pair3);

    println("pair1={}", pair1);
}

void test_vector(void) {
    using std::string;
    using MyVec = my::vector<string, my::allocator<string>>;

    auto my_alloc = my::allocator<string>();
    MyVec vec1{my_alloc};
    vec1.show();

    MyVec vec2{10, "urmom", my_alloc};
    vec2.show();

    MyVec vec3{vec2.begin(), vec2.end()};
    vec3.show();

    MyVec vec4 = std::move(vec3);
    vec4.show();

    my::vector<int> vec5 = {1, 2, 3, 4, 5};
    vec5.show();

    println("Expected 5, got {}", vec5.at(4));
    println("Expected 5, got {}", vec5[4]);
    println("UB {}", vec5[100]);
    try {
        vec5.at(100);
    } catch (const std::out_of_range& e) {
        println("Exception caught: {}", e.what());
    }

    vec5.push_back(6);
    vec5.show();
    // vec5.shrink_to_fit();

    vec5.insert(vec5.end(), 42);
    vec5.show();
}

void test_optional(void) {
    using std::string;
    using my::optional;

    optional<string> some_str {"hello world"};
    optional<string> none {};
    string str {"hellow world"};

    assert(some_str && (!none));
    println("sizeof some={}, sizeof none={}, sizeof str={}", sizeof(some_str), sizeof(none), sizeof(str));

    optional<string> move_to = std::move(some_str);
    assert(some_str && move_to);

    optional<string> copy_to = move_to;
    assert(move_to && copy_to);

    none.swap(move_to);
    assert((!move_to) && none);
    assert(none.unwrap() == "hello world");

    auto res = copy_to
        .and_then([](string s) {
            assert(s == "hello world");
            return optional<string> {"foo"};
        })
        .transform([](string s) -> string {
            assert(s == "foo");
            return "bar";
        })
        .or_else([]() -> string {
            assert(false);
            return "unreachable";
        });

    assert(res.unwrap() == "bar");

    optional<int> some_int { 42 };
    assert(some_int.is_some_and([](int x) { return x > 30; }));
    assert((optional<int> {}).is_none_or([](int) { return false; }));
    println("{}, {}, {}", some_int, res, move_to);
}
