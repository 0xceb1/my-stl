#include <memory>
#include <print>
#include <string>
#include <vector>
#include <cassert>
#include "my/memory.hpp"
#include "my/utility.hpp"

using std::println;

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
}

int main(void) {
    // test_allocator();
    test_pair();
}
