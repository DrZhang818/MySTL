#include <cassert>
#include <iostream>
#include <string>

#include "type_traits.h"
#include "utility.h"

struct Tracker {
    enum Type { NONE, LVALUE, RVALUE };
    Type last_op = NONE;

    Tracker() = default;
    Tracker(const Tracker&) : last_op(LVALUE) {}
    Tracker(Tracker&&) : last_op(RVALUE) {}
};

void test_move() {
    std::cout << "Testing move..." << std::endl;

    int x = 10;
    static_assert(mystl::is_rvalue_reference_v<decltype(mystl::move(x))>);

    const int cx = 20;
    static_assert(mystl::is_rvalue_reference_v<decltype(mystl::move(cx))>);
    static_assert(mystl::is_const_v<mystl::remove_reference_t<decltype(mystl::move(cx))>>);

    int y = 100;
    int&& ry = mystl::move(y);
    assert(ry == 100);

    std::cout << "move tests passed." << std::endl;
}

void test_forward() {
    std::cout << "Testing forward..." << std::endl;

    Tracker t;

    auto&& res1 = mystl::forward<Tracker&>(t);
    static_assert(mystl::is_lvalue_reference_v<decltype(res1)>);

    auto&& res2 = mystl::forward<Tracker>(Tracker());
    static_assert(mystl::is_rvalue_reference_v<decltype(res2)>);

    auto&& res3 = mystl::forward<Tracker>(mystl::move(t));
    static_assert(mystl::is_rvalue_reference_v<decltype(res3)>);

    std::cout << "forward tests passed." << std::endl;
}

void test_constexpr_noexcept() {
    constexpr int x = 5;
    constexpr int y = mystl::move(x);
    static_assert(y == 5);

    int z = 1;
    static_assert(noexcept(mystl::move(z)));
    static_assert(noexcept(mystl::forward<int>(z)));
}

void test_exchange() {
    std::cout << "Testing exchange..." << std::endl;

    int a = 1;
    int b = mystl::exchange(a, 2);
    assert(a == 2);
    assert(b == 1);

    std::string s1 = "Hello";
    std::string s2 = mystl::exchange(s1, "World");
    assert(s1 == "World");
    assert(s2 == "Hello");

    static_assert(mystl::is_nothrow_move_constructible_v<int>);

    static_assert(mystl::is_nothrow_assignable_v<int&, int>);

    static_assert(mystl::is_nothrow_move_constructible_v<int> &&
                  mystl::is_nothrow_assignable_v<int&, int>);

    static_assert(noexcept(mystl::exchange(a, 3)));
    std::cout << "exchange tests passed." << std::endl;
}

void test_to_underlying() {
    std::cout << "Testing to_underlying..." << std::endl;

    enum class Color : unsigned char { Red = 0, Blue = 255 };
    auto val = mystl::to_underlying(Color::Blue);

    static_assert(std::is_same_v<decltype(val), unsigned char>);
    assert(val == 255);

    std::cout << "to_underlying tests passed." << std::endl;
}

void test_forward_like() {
    std::cout << "Testing forward_like..." << std::endl;

    int x = 0;
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<int&>(x)), int&>);
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<const int&>(x)), const int&>);
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<int&&>(x)), int&&>);
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<const int&&>(x)), const int&&>);

    std::cout << "forward_like tests passed." << std::endl;
}

int main() {
    test_move();
    test_forward();
    test_constexpr_noexcept();
    test_exchange();
    test_to_underlying();
    test_forward_like();

    std::cout << "\n[SUCCESS]: Move and Forward are working correctly!" << std::endl;

    return 0;
}