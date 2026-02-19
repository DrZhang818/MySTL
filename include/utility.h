#ifndef MYSTL_HADEMADE_UTILITY_H_
#define _MYSTL_HADEMADE_UTILITY_H_

#include <compare>
#include <limits>

#include "type_traits.h"

namespace mystl {
template <typename T>
constexpr remove_reference_t<T>&& move(T&& arg) noexcept {
    return static_cast<remove_reference_t<T>&&>(arg);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& arg) noexcept {
    static_assert(!is_lvalue_reference_v<T>,
                  "mystl::forward requires non-lvalue-reference T to forward as rvalue");
    return static_cast<T&&>(arg);
}

template <typename T, typename U = T>
constexpr T exchange(T& obj, U&& new_value) noexcept(is_nothrow_move_constructible_v<T> &&
                                                     is_nothrow_assignable_v<T&, U>) {
    T old_value = mystl::move(obj);
    obj = mystl::forward<U>(new_value);
    return old_value;
}

template <typename T>
    requires(is_enum_v<T>)
constexpr auto to_underlying(T value) noexcept {
    return static_cast<__underlying_type(T)>(value);
}

[[noreturn]] inline void unreachable() noexcept {
#if defined(__GNUC__) || defined(__clang__)
    __builtin_unreachable();
#elif defined(_MSC_VER)
    __assume(false);
#endif
}

template <typename T, typename U>
constexpr auto&& forward_like(U&& arg) noexcept {
    constexpr bool is_adding_const = is_const_v<remove_reference_t<T>>;
    if constexpr (is_lvalue_reference_v<T>) {
        if constexpr (is_adding_const) {
            return static_cast<const remove_reference_t<U>&>(arg);
        } else {
            return static_cast<remove_reference_t<U>&>(arg);
        }
    } else {
        if constexpr (is_adding_const) {
            return static_cast<const remove_reference_t<U>&&>(arg);
        } else {
            return static_cast<remove_reference_t<U>&&>(arg);
        }
    }
}

}  // namespace mystl

#endif