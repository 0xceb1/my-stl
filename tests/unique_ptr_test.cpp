#include <string>
#include <memory>
#include <print>
#include <type_traits>
#include <utility>
#include <catch2/catch_test_macros.hpp>
#include "my/memory.hpp"

using T = std::string;
using Deleter = my::default_delete<T>;

struct CountingDeleter {
    inline static int delete_count = 0;
    int id = 0; // dummy member
    void operator()(T* p) const {
        ++delete_count;
        delete p;
    }
    static void reset() { delete_count = 0; }
};

Deleter make_deleter() {
    return Deleter();
}

using Ptr = my::unique_ptr<T, Deleter>;
using CountPtr = my::unique_ptr<T, CountingDeleter>;

TEST_CASE("my::default_delete works as expected", "[my::unique_ptr]") {
    static_assert(std::is_empty_v<Deleter> && (!std::is_final_v<Deleter>), "Deleter can be EBO");
    static_assert(!std::is_empty_v<CountingDeleter>, "CountingDeleter cannot be EBO");
    REQUIRE(sizeof(Deleter) == 1);
    REQUIRE(sizeof(CountingDeleter) > sizeof(Deleter));
}

TEST_CASE("my::unique_ptr can be constructed", "[my::unique_ptr]") {
    SECTION("default constructed") {
        Ptr p1{};
        Ptr p2{nullptr};
        CountPtr p3{};

        REQUIRE(sizeof(p1) == sizeof(T*));
        REQUIRE(sizeof(p2) == sizeof(T*));
        REQUIRE(sizeof(p3) > sizeof(T*));
        REQUIRE_FALSE(p1);
        REQUIRE_FALSE(p2);
        REQUIRE(p1.get() == nullptr);
    }
    SECTION("construct from a pointer") {
        Ptr p{new T("foo")};
        REQUIRE(sizeof(p) == sizeof(T*));
        REQUIRE(p);
        REQUIRE(*p == "foo");
    }
    SECTION("construct from pointer and deleter") {
        Ptr p{new T("foo"), make_deleter()};
        REQUIRE(sizeof(p) == sizeof(T*));
        REQUIRE(*p == "foo");
    }
    SECTION("move constructed") {
        Ptr p1{new T("bar")};
        T* raw1 = p1.get();

        Ptr p2{std::move(p1)};

        REQUIRE_FALSE(p1);
        REQUIRE(p1.get() == nullptr);
        REQUIRE(p2.get() == raw1);
        REQUIRE(*p2 == "bar");
    }
}

TEST_CASE("my::unique_ptr assignment operators", "[my::unique_ptr]") {
    SECTION("move assignment") {
        Ptr p1{new T("hello")};
        Ptr p2{new T("world")};

        T* raw1 = p1.get();

        p2 = std::move(p1);

        REQUIRE_FALSE(p1);
        REQUIRE(p2.get() == raw1);
        REQUIRE(*p2 == "hello");
    }
    SECTION("nullptr assignment") {
        CountingDeleter::reset();
        {
            CountPtr p{new T("test")};
            REQUIRE(p);

            p = nullptr;

            REQUIRE_FALSE(p);
            REQUIRE(CountingDeleter::delete_count == 1);
        }
    }
}

TEST_CASE("my::unique_ptr observers", "[my::unique_ptr]") {
    SECTION("get() returns raw pointer") {
        T* raw = new T("test");
        Ptr p{raw};

        REQUIRE(p.get() == raw);
    }
    SECTION("operator bool") {
        Ptr p1{};
        Ptr p2{new T("test")};

        REQUIRE_FALSE(p1);
        REQUIRE(p2);
    }
    SECTION("operator* and operator->") {
        Ptr p{new T("hello")};

        REQUIRE(*p == "hello");
        REQUIRE(p->size() == 5);

        *p = "world";
        REQUIRE(*p == "world");
    }
    SECTION("get_deleter") {
        CountingDeleter d;
        CountPtr p{new T("test"), d};

        (void)p.get_deleter();
        REQUIRE(true);
    }
}

TEST_CASE("my::unique_ptr modifiers", "[my::unique_ptr]") {
    SECTION("release") {
        Ptr p{new T("test")};

        T* raw = p.release();

        REQUIRE_FALSE(p);
        REQUIRE(p.get() == nullptr);
        REQUIRE(*raw == "test");

        delete raw;
    }
    SECTION("reset with no argument") {
        CountingDeleter::reset();
        {
            CountPtr p{new T("test")};

            p.reset();

            REQUIRE_FALSE(p);
            REQUIRE(CountingDeleter::delete_count == 1);
        }
    }
    SECTION("reset with new pointer") {
        CountingDeleter::reset();
        {
            CountPtr p{new T("old")};
            T* new_raw = new T("new");

            p.reset(new_raw);

            REQUIRE(p);
            REQUIRE(*p == "new");
            REQUIRE(CountingDeleter::delete_count == 1);
        }
        REQUIRE(CountingDeleter::delete_count == 2);
    }
    SECTION("swap") {
        Ptr p1{new T("first")};
        Ptr p2{new T("second")};

        T* raw1 = p1.get();
        T* raw2 = p2.get();

        p1.swap(p2);

        REQUIRE(p1.get() == raw2);
        REQUIRE(p2.get() == raw1);
        REQUIRE(*p1 == "second");
        REQUIRE(*p2 == "first");
    }
}

TEST_CASE("my::unique_ptr comparison operators", "[my::unique_ptr]") {
    SECTION("equality with another unique_ptr") {
        Ptr p1{new T("test")};
        Ptr p2{new T("test")};
        Ptr p3{};

        REQUIRE(p1 != p2);
        REQUIRE(p1 == p1);
        REQUIRE(p3 == Ptr{});
    }
    SECTION("equality with nullptr") {
        Ptr p1{};
        Ptr p2{new T("test")};

        REQUIRE(p1 == nullptr);
        REQUIRE(nullptr == p1);
        REQUIRE(p2 != nullptr);
        REQUIRE(nullptr != p2);
    }
    SECTION("three-way comparison") {
        T* raw1 = new T("a");
        T* raw2 = new T("b");
        Ptr p1{raw1};
        Ptr p2{raw2};

        auto cmp = p1 <=> p2;
        auto ptr_cmp = raw1 <=> raw2;
        REQUIRE(cmp == ptr_cmp);

        Ptr p3{};
        REQUIRE((p3 <=> nullptr) == std::strong_ordering::equal);
    }
}

TEST_CASE("my::make_unique works correctly", "[my::unique_ptr]") {
    SECTION("make_unique with arguments") {
        auto p = my::make_unique<T>("hello");

        REQUIRE(p);
        REQUIRE(*p == "hello");
    }
    SECTION("make_unique default construction") {
        auto p = my::make_unique<T>();

        REQUIRE(p);
        REQUIRE(*p == "");
    }
    SECTION("make_unique_for_overwrite") {
        auto p = my::make_unique_for_overwrite<int>();

        REQUIRE(p);
    }
}

TEST_CASE("my::unique_ptr with derived types", "[my::unique_ptr]") {
    struct Base {
        virtual ~Base() = default;
        virtual int value() const { return 0; }
    };
    struct Derived : Base {
        int value() const override { return 42; }
    };

    SECTION("converting constructor from derived to base") {
        my::unique_ptr<Derived> pd{new Derived()};

        my::unique_ptr<Base> pb{std::move(pd)};

        REQUIRE_FALSE(pd);
        REQUIRE(pb->value() == 42);
    }
    SECTION("converting assignment from derived to base") {
        my::unique_ptr<Derived> pd{new Derived()};

        my::unique_ptr<Base> pb{new Base()};

        pb = std::move(pd);

        REQUIRE_FALSE(pd);
        REQUIRE(pb->value() == 42);
    }
}

TEST_CASE("my::unique_ptr destructor calls deleter", "[my::unique_ptr]") {
    CountingDeleter::reset();
    REQUIRE(CountingDeleter::delete_count == 0);

    {
        CountPtr p{new T("test")};
    }

    REQUIRE(CountingDeleter::delete_count == 1);
}

TEST_CASE("my::unique_ptr with custom deleter by value", "[my::unique_ptr]") {
    CountingDeleter::reset();

    SECTION("deleter is called on destruction") {
        {
            CountPtr p{new T("test"), CountingDeleter{}};
        }
        REQUIRE(CountingDeleter::delete_count == 1);
    }
    SECTION("deleter is called on reset") {
        CountPtr p{new T("test"), CountingDeleter{}};
        p.reset();
        REQUIRE(CountingDeleter::delete_count == 1);
    }
}
