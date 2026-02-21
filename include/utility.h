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

template <typename T>
constexpr add_const_t<T>& as_const(T& arg) noexcept {
    return arg;
}

template <typename T>
void as_const(const T&&) = delete;

template <typename T>
    requires(is_move_constructible_v<T> && is_move_assignable_v<T>)
constexpr void swap(T& a, T& b) noexcept(is_nothrow_move_constructible_v<T> &&
                                         is_nothrow_move_assignable_v<T>) {
    T temp = mystl::move(a);
    a = mystl::move(b);
    b = mystl::move(temp);
}

template <typename T>
concept Swappable = requires(T& a, T& b) { mystl::swap(a, b); };

template <Swappable T, size_t N>
constexpr void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(mystl::swap(a[0], b[0]))) {
    for (size_t i = 0; i < N; ++i) { mystl::swap(a[i], b[i]); }
}

template <typename T, typename U>
    requires(is_integral_v<T> && is_integral_v<U>)
constexpr bool cmp_equal(T lhs, U rhs) noexcept {
    if constexpr (is_signed_v<T> == is_signed_v<U>) {
        return lhs == rhs;
    } else if constexpr (is_signed_v<T>) {
        return lhs < 0 ? false : static_cast<make_unsigned_t<T>>(lhs) == rhs;
    } else {
        return rhs < 0 ? false : lhs == static_cast<make_unsigned_t<U>>(rhs);
    }
}

template <typename T, typename U>
    requires(is_integral_v<T> && is_integral_v<U>)
constexpr bool cmp_not_equal(T lhs, U rhs) noexcept {
    return !mystl::cmp_equal(lhs, rhs);
}

template <typename T, typename U>
    requires(is_integral_v<T> && is_integral_v<U>)
constexpr bool cmp_less(T lhs, U rhs) noexcept {
    if constexpr (is_signed_v<T> == is_signed_v<U>) {
        return lhs < rhs;
    } else if constexpr (is_signed_v<T>) {
        return lhs < 0 ? true : static_cast<make_unsigned_t<T>>(lhs) < rhs;
    } else {
        return rhs < 0 ? false : lhs < static_cast<make_unsigned_t<U>>(rhs);
    }
}

template <typename T, typename U>
constexpr bool cmp_greater(T lhs, U rhs) noexcept {
    return mystl::cmp_less(rhs, lhs);
}

template <typename T, typename U>
constexpr bool cmp_less_equal(T lhs, U rhs) noexcept {
    return !mystl::cmp_less(rhs, lhs);
}

template <typename T, typename U>
constexpr bool cmp_greater_equal(T lhs, U rhs) noexcept {
    return !mystl::cmp_less(lhs, rhs);
}

template <typename T, typename U>
    requires(is_integral_v<T> && is_integral_v<U>)
constexpr bool in_range(T value) noexcept {
    return mystl::cmp_greater_equal(value, std::numeric_limits<U>::min()) &&
           mystl::cmp_less_equal(value, std::numeric_limits<U>::max());
}

template <typename T>
    requires(is_integral_v<T>)
constexpr T byteswap(T value) noexcept {
    if constexpr (sizeof(T) == 1) {
        return value;
    } else if constexpr (sizeof(T) == 2) {
        return static_cast<T>(((value & 0x00FF) << 8) | ((value & 0xFF00) >> 8));
    } else if constexpr (sizeof(T) == 4) {
        return static_cast<T>(((value & 0x000000FF) << 24) | ((value & 0x0000FF00) << 8) |
                              ((value & 0x00FF0000) >> 8) | ((value & 0xFF000000) >> 24));
    } else if constexpr (sizeof(T) == 8) {
        return static_cast<T>(
            ((value & 0x00000000000000FFULL) << 56) | ((value & 0x000000000000FF00ULL) << 40) |
            ((value & 0x0000000000FF0000ULL) << 24) | ((value & 0x00000000FF000000ULL) << 8) |
            ((value & 0x000000FF00000000ULL) >> 8) | ((value & 0x0000FF0000000000ULL) >> 24) |
            ((value & 0x00FF000000000000ULL) >> 40) | ((value & 0xFF00000000000000ULL) >> 56));
    } else {
        static_assert(sizeof(T) <= 8, "byteswap only supports integral types up to 64 bits");
    }
}

template <typename T, T... Ints>
struct integer_sequence {
    using value_type = T;
    static constexpr size_t size() noexcept { return sizeof...(Ints); }
};

template <size_t... Ints>
using index_sequence = integer_sequence<size_t, Ints...>;

namespace detail {
template <typename T, size_t N, T... Ints>
struct make_seq_impl : make_seq_impl<T, N - 1, static_cast<T>(N - 1), Ints...> {};

template <typename T, T... Ints>
struct make_seq_impl<T, 0, Ints...> {
    using type = integer_sequence<T, Ints...>;
};
}  // namespace detail

#if defined(__clang__) || defined(_MSC_VER)
template <typename T, T N>
using make_integer_sequence = __make_integer_seq<integer_sequence, T, N>;

#elif defined(__GNUC__) && __GNUC__ >= 8
template <typename T, T N>
using make_integer_sequence = integer_sequence<T, __integer_pack(N)...>;

#else
template <typename T, T N>
using make_integer_sequence = typename detail::make_seq_impl<T, N>::type;
#endif

template <size_t N>
using make_index_sequence = make_integer_sequence<size_t, N>;

template <typename... Args>
using index_sequence_for = make_index_sequence<sizeof...(Args)>;


}  // namespace mystl

#endif