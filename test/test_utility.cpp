#include <cassert>
#include <iostream>
#include <string>

#include "type_traits.h"
#include "utility.h"

#define TEST_CASE(name) std::cout << "[RUNNING] " << name << "..." << std::endl;
#define TEST_CASE_PASS(name) std::cout << "[PASSED] " << name << std::endl;

struct Tracker {
    enum Type { NONE, LVALUE, RVALUE };
    Type last_op = NONE;

    Tracker() = default;
    Tracker(const Tracker&) : last_op(LVALUE) {}
    Tracker(Tracker&&) : last_op(RVALUE) {}
};

void test_move() {
    TEST_CASE("move");

    int x = 10;
    static_assert(mystl::is_rvalue_reference_v<decltype(mystl::move(x))>);

    const int cx = 20;
    static_assert(mystl::is_rvalue_reference_v<decltype(mystl::move(cx))>);
    static_assert(mystl::is_const_v<mystl::remove_reference_t<decltype(mystl::move(cx))>>);

    int y = 100;
    int&& ry = mystl::move(y);
    assert(ry == 100);

    TEST_CASE_PASS("move");
}

void test_forward() {
    TEST_CASE("forward");

    Tracker t;

    auto&& res1 = mystl::forward<Tracker&>(t);
    static_assert(mystl::is_lvalue_reference_v<decltype(res1)>);

    auto&& res2 = mystl::forward<Tracker>(Tracker());
    static_assert(mystl::is_rvalue_reference_v<decltype(res2)>);

    auto&& res3 = mystl::forward<Tracker>(mystl::move(t));
    static_assert(mystl::is_rvalue_reference_v<decltype(res3)>);

    TEST_CASE_PASS("forward");
}

void test_constexpr_noexcept() {
    TEST_CASE("constexpr and noexcept");

    constexpr int x = 5;
    constexpr int y = mystl::move(x);
    static_assert(y == 5);

    int z = 1;
    static_assert(noexcept(mystl::move(z)));
    static_assert(noexcept(mystl::forward<int>(z)));

    TEST_CASE_PASS("constexpr and noexcept");
}

void test_exchange() {
    TEST_CASE("exchange");

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

    TEST_CASE_PASS("exchange");
}

void test_to_underlying() {
    TEST_CASE("to_underlying");

    enum class Color : unsigned char { Red = 0, Blue = 255 };
    auto val = mystl::to_underlying(Color::Blue);

    static_assert(std::is_same_v<decltype(val), unsigned char>);
    assert(val == 255);

    TEST_CASE_PASS("to_underlying");
}

void test_forward_like() {
    TEST_CASE("forward_like");

    int x = 0;
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<int&>(x)), int&>);
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<const int&>(x)), const int&>);
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<int&&>(x)), int&&>);
    static_assert(mystl::is_same_v<decltype(mystl::forward_like<const int&&>(x)), const int&&>);

    TEST_CASE_PASS("forward_like");
}

struct MoveOnly {
    int data;
    MoveOnly(int d) : data(d) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&& other) noexcept : data(mystl::exchange(other.data, 0)) {}
    MoveOnly& operator=(MoveOnly&& other) noexcept {
        data = mystl::exchange(other.data, 0);
        return *this;
    }
};

void test_as_const() {
    TEST_CASE("as_const");

    int x = 10;
    auto& cx = mystl::as_const(x);
    static_assert(mystl::is_const_v<mystl::remove_reference_t<decltype(cx)>>,
                  "as_const should return const ref)");
    assert(&x == &cx);
    // mystl::as_const(10);

    TEST_CASE_PASS("as_const");
}

void test_swap_scalar() {
    TEST_CASE("swap scalar");

    int a = 1, b = 2;
    mystl::swap(a, b);
    assert(a == 2 && b == 1);

    MoveOnly m1(10), m2(20);
    mystl::swap(m1, m2);
    assert(m1.data == 20 && m2.data == 10);

    TEST_CASE_PASS("swap scalar");
}

void test_swap_array() {
    TEST_CASE("swap array");

    int arr1[3] = {1, 2, 3};
    int arr2[3] = {4, 5, 6};
    mystl::swap(arr1, arr2);
    assert(arr1[0] == 4 && arr1[1] == 5 && arr1[2] == 6);
    assert(arr2[0] == 1 && arr2[1] == 2 && arr2[2] == 3);

    TEST_CASE_PASS("swap array");
}

int main() {
    test_move();
    test_forward();
    test_constexpr_noexcept();
    test_exchange();
    test_to_underlying();
    test_forward_like();
    test_as_const();
    test_swap_scalar();
    test_swap_array();

    return 0;
}