#ifndef MYSTL_HANDMADE_CONCEPTS_H_
#define MYSTL_HANDMADE_CONCEPTS_H_

#include "type_traits.h"
#include "utility.h"

namespace mystl {

template <typename T, typename U>
concept same_as = is_same_v<T, U> && is_same_v<U, T>;

template <typename Derived, typename Base>
concept derived_from =
    is_base_of_v<Base, Derived> && is_convertible_v<const volatile Derived*, const volatile Base*>;

template <typename From, typename To>
concept convertible_to =
    is_convertible_v<From, To> && requires { static_cast<To>(declval<From>()); };

template <typename T>
concept integral = is_integral_v<T>;

template <typename T>
concept signed_integral = integral<T> && is_signed_v<T>;

template <typename T>
concept unsigned_integral = integral<T> && !is_signed_v<T>;

template <typename T>
concept floating_point = is_floating_point_v<T>;

template <typename T>
concept reference = is_reference_v<T>;

template <typename T>
concept scalar = is_scalar_v<T>;

template <typename T>
concept destructible = is_nothrow_destructible_v<T>;

template <typename T, typename... Args>
concept constructible_from = destructible<T> && is_constructible_v<T, Args...>;

template <typename T>
concept default_initializable = constructible_from<T> && requires { T{}; } && requires { ::new T; };

template <typename T>
concept move_constructible = constructible_from<T, T> && convertible_to<T, T>;

template <typename T>
concept copy_constructible =
    move_constructible<T> && constructible_from<T, T&> && convertible_to<T&, T> &&
    constructible_from<T, const T&> && convertible_to<const T&, T> &&
    constructible_from<T, const T> && convertible_to<const T, T>;

template <typename LHS, typename RHS>
concept assignable_from = is_lvalue_reference_v<LHS> && requires(LHS lhs, RHS&& rhs) {
    { lhs = mystl::forward<RHS>(rhs) } -> same_as<LHS>;
};

template <typename T>
concept swappable = is_swappable_v<T>;

template <typename T>
concept movable = is_object_v<T> && move_constructible<T> && assignable_from<T&, T> && swappable<T>;

template <typename T>
concept copyable = copy_constructible<T> && movable<T> && assignable_from<T&, T&> &&
                   assignable_from<T&, const T&> && assignable_from<T&, const T>;

template <typename T>
concept semiregular = copyable<T> && default_initializable<T>;

template <typename T>
concept equality_comparable =
    requires(const remove_reference_t<T>& a, const remove_reference_t<T>& b) {
        { a == b } -> convertible_to<bool>;
        { a != b } -> convertible_to<bool>;
    };

template <typename T>
concept regular = semiregular<T> && equality_comparable<T>;

}  // namespace mystl

#endif