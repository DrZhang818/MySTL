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
    int val;
    MoveOnly(int d) : val(d) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&& other) noexcept : val(mystl::exchange(other.val, 0)) {}
    MoveOnly& operator=(MoveOnly&& other) noexcept {
        val = mystl::exchange(other.val, 0);
        return *this;
    }
    bool operator==(const MoveOnly& rhs) const { return val == rhs.val; }
    auto operator<=>(const MoveOnly& rhs) const { return val <=> rhs.val; }
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
    assert(m1.val == 20 && m2.val == 10);

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

void test_make_index_sequence() {
    TEST_CASE("make_index_sequence");

    static_assert(mystl::is_same_v<mystl::make_index_sequence<3>, mystl::index_sequence<0, 1, 2>>,
                  "make_index_sequence failed");

    static_assert(mystl::is_same_v<mystl::make_index_sequence<0>, mystl::index_sequence<>>,
                  "make_index_sequence<0> failed");

    static_assert(mystl::is_same_v<mystl::index_sequence_for<int, double, char>,
                                   mystl::index_sequence<0, 1, 2>>,
                  "index_sequence_for failed");

    TEST_CASE_PASS("make_index_sequence");
}

void test_in_place() {
    TEST_CASE("in_place");

    auto tag1 = mystl::in_place;
    auto tag2 = mystl::in_place_type<int>;
    auto tag3 = mystl::in_place_index<0>;

    static_assert(sizeof(mystl::in_place_t) == 1, "in_place_t should be empty");
    static_assert(sizeof(mystl::in_place_type_t<int>) == 1, "in_place_type_t should be empty");
    static_assert(sizeof(mystl::in_place_index_t<42>) == 1, "in_place_index_t should be empty");

    TEST_CASE_PASS("in_place");
}

struct ExplicitType {
    int val;
    explicit ExplicitType(int v) : val(v) {}
};

void test_pair_constructors() {
    TEST_CASE("pair_constructors");

    mystl::pair<int, std::string> p1;
    assert(p1.first == 0);
    assert(p1.second == "");

    mystl::pair<int, double> p2(42, 3.14);
    assert(p2.first == 42);
    assert(p2.second == 3.14);

    mystl::pair<std::string, MoveOnly> p3("Hello", MoveOnly(100));
    assert(p3.first == "Hello");
    assert(p3.second.val == 100);

    // auto p4 = p3;

    mystl::pair<int, int> p5(1, 2);
    mystl::pair<int, int> p6(p5);
    assert(p6.first == 1 && p6.second == 2);

    mystl::pair<int, int> p7(std::move(p5));
    assert(p7.first == 1);

    TEST_CASE_PASS("pair_constructors");
}

void test_pair_conversion() {
    TEST_CASE("pair_conversions");

    mystl::pair<int, int> p1(10, 20);
    mystl::pair<double, double> p2 = p1;
    assert(p2.first == 10.0 && p2.second == 20.0);

    static_assert(!mystl::is_convertible_v<int, ExplicitType>,
                  "int should not be convertible to ExplicitType");

    mystl::pair<ExplicitType, int> p4(ExplicitType{10}, 20);
    assert(p4.first.val == 10);

    TEST_CASE_PASS("pair_conversions");
}

void test_pair_assignment() {
    TEST_CASE("pair_assignment");

    mystl::pair<int, std::string> p1(1, "old");
    mystl::pair<int, std::string> p2(2, "new");

    p1 = p2;
    assert(p1.first == 2 && p1.second == "new");

    p1 = mystl::make_pair(3, "move");
    assert(p1.first == 3 && p1.second == "move");

    mystl::pair<double, const char*> p3(1.1, "hi");
    mystl::pair<int, std::string> p4;
    p4 = p3;
    assert(p4.first == 1 && p4.second == "hi");

    TEST_CASE_PASS("pair_assignment");
}

void test_pair_comparison() {
    TEST_CASE("pair_comparison");

    mystl::pair<int, int> p1(1, 10);
    mystl::pair<int, int> p2(1, 20);
    mystl::pair<int, int> p3(2, 5);

    assert(p1 == p1);
    assert(p1 != p2);
    assert(p1 < p2);
    assert(p2 < p3);
    assert(p3 > p1);
    assert(p1 <= p2);

    mystl::pair<double, double> p4(1.0, 15.0);
    assert(p1 < p4);

    TEST_CASE_PASS("pair_comparison");
}

void test_pair_tuple_interface() {
    TEST_CASE("pair_tuple_interface");

    mystl::pair<int, std::string> p(42, "answer");

    assert(mystl::get<0>(p) == 42);
    assert(mystl::get<1>(p) == "answer");

    assert(mystl::get<int>(p) == 42);
    assert(mystl::get<std::string>(p) == "answer");

    mystl::get<0>(p) = 7;
    assert(p.first == 7);

    static_assert(mystl::tuple_size<decltype(p)>::value == 2);
    static_assert(mystl::is_same_v<mystl::tuple_element_t<0, decltype(p)>, int>);

    TEST_CASE_PASS("pair_tuple_interface");
}

void test_pair_make_pair() {
    TEST_CASE("pair_make_pair");

    auto a = int(10);
    auto p1 = mystl::make_pair(a, "hello");

    static_assert(mystl::is_same_v<decltype(p1), mystl::pair<int, const char*>>);
    assert(p1.first == 10);

    TEST_CASE_PASS("pair_make_pair");
}

void test_pair_swap() {
    TEST_CASE("pair_swap");

    mystl::pair<int, int> p1(1, 2);
    mystl::pair<int, int> p2(3, 4);

    mystl::swap(p1, p2);

    assert(p1.first == 3 && p1.second == 4);
    assert(p2.first == 1 && p2.second == 2);

    TEST_CASE_PASS("pair_swap");
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
    test_make_index_sequence();
    test_in_place();
    test_pair_constructors();
    test_pair_conversion();
    test_pair_assignment();
    test_pair_comparison();
    test_pair_tuple_interface();
    test_pair_make_pair();
    test_pair_swap();

    return 0;
}