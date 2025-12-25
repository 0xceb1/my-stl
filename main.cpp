#include <memory>
#include <print>
#include <string>
#include <vector>
#include "my/memory.hpp"

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

int main(void) {
    test_allocator();
}
