#include <iostream>
#include "type_traits.h"

namespace mystl {
    template <typename T1, typename T2>
    struct pair {
        T1 first;
        T2 second;
    };
}

struct TrivialStruct {
    int x;
    float y;
};

struct NonTrivialStruct {
    int* ptr;
    NonTrivialStruct() { ptr = new int(0); }
    ~NonTrivialStruct() { delete ptr; }
};

void ref_check(int&) {
    std::cout << "Lvalue reference check Passed!" << std::endl;
}

void ref_check(int&&) {
    std::cout << "Rvalue reference check Passed!" << std::endl;
}

template <typename T>
void wrapper(T&& arg) {
    ref_check(mystl::forward<T>(arg));
}

int main() {
    std::cout << "Starting type traits tests..." << std::endl;

    static_assert(mystl::true_type::value == true, "true_type test failed");
    static_assert(mystl::false_type::value == false, "false_type test failed");

    static_assert(mystl::is_same_v<mystl::conditional_t<true, int, float>, int> == true, "conditional test 1 failed");
    static_assert(mystl::is_same_v<mystl::conditional_t<false, int, float>, float> == true, "conditional test 2 failed");

    static_assert(mystl::is_same_v<int, int> == true, "is_same<int, int> test failed");
    static_assert(mystl::is_same_v<int, float> == false, "is_same<int, float> test failed");

    std::cout << "Basic Helpers (true_type, false_type, is_same) tests passed!" << std::endl;

    static_assert(mystl::is_same_v<mystl::remove_reference_t<int>,int>, "remove_reference<int> test failed");
    static_assert(mystl::is_same_v<mystl::remove_reference_t<int&>, int>, "remove_reference<int&> test failed");
    static_assert(mystl::is_same_v<mystl::remove_reference_t<int&&>, int>, "remove_reference<int&&> test failed");

    static_assert(mystl::is_same_v<mystl::remove_const_t<const int>, int>, "remove_const<const int> test failed");
    static_assert(mystl::is_same_v<mystl::remove_volatile_t<volatile int>, int>, "remove_volatile<volatile int> test failed");

    static_assert(mystl::is_same_v<mystl::remove_cv_t<const volatile int>, int>, "remove_cv<const volatile int> test failed");

    std::cout << "Type Modifications (remove_reference, remove_const, remove_volatile, remove_cv) tests passed!" << std::endl;

    static_assert(mystl::is_pair_v<mystl::pair<int, double>> == true, "is_pair<pair<int, double>> test failed");
    static_assert(mystl::is_pair_v<int> == false, "is_pair<int> negative test failed");

    static_assert(mystl::is_pair_v<const mystl::pair<int, int>> == true, "is_pair<const pair<int, int>> test failed");

    std::cout << "is_pair tests passed!" << std::endl;

    static_assert(mystl::is_trivially_destructible_v<int> == true, "int should be trivial");
    static_assert(mystl::is_trivially_destructible_v<TrivialStruct> == true, "Strcut should be trivial");
    static_assert(mystl::is_trivially_destructible_v<NonTrivialStruct> == false, "Class with destructor should NOT be trivial");

    std::cout << "Compiler Intrinsics (is_trivially_destructible) tests passed!" << std::endl;

    std::cout << "Testing forward and move semantics..." << std::endl;
    int x = 42;
    wrapper(x);
    wrapper(42);
    wrapper(mystl::move(x));

    static_assert(mystl::is_same_v<decltype(mystl::move(x)), int&&>, "move return type test failed");

    std::cout << "\nAll Compile-Time Assertions Passed!" << std::endl;
    
    return 0;
}