#ifndef MYSTL_HADEMADE_UTILITY_H_
#define MYSTL_HADEMADE_UTILITY_H_

#include <compare>
#include <limits>

#include "type_traits.h"

namespace mystl {

template <typename T1, typename T2>
struct pair;

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

#include "concepts.h"
namespace mystl {
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

template <typename T, size_t N>
    requires requires(T& a, T& b) { mystl::swap(a, b); }
constexpr void swap(T (&a)[N], T (&b)[N]) noexcept(noexcept(mystl::swap(a[0], b[0]))) {
    for (size_t i = 0; i < N; ++i) {
        mystl::swap(a[i], b[i]);
    }
}

template <swappable T1, swappable T2>
constexpr void swap(pair<T1, T2>& x, pair<T1, T2>& y) noexcept(is_nothrow_swappable_v<T1> &&
                                                               is_nothrow_swappable_v<T2>) {
    using mystl::swap;
    swap(x.first, y.first);
    swap(x.second, y.second);
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

struct in_place_t {
    explicit in_place_t() = default;
};
inline constexpr in_place_t in_place{};

template <typename T>
struct in_place_type_t {
    explicit in_place_type_t() = default;
};

template <typename T>
inline constexpr in_place_type_t<T> in_place_type{};

template <size_t I>
struct in_place_index_t {
    explicit in_place_index_t() = default;
};

template <size_t I>
inline constexpr in_place_index_t<I> in_place_index{};

template <typename T1, typename T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    T1 first;
    T2 second;

    constexpr pair()
        requires(is_default_constructible_v<T1> && is_default_constructible_v<T2>)
        : first(), second() {};

    constexpr explicit(!is_convertible_v<const T1&, T1> || !is_convertible_v<const T2&, T2>)
        pair(const T1& x, const T2& y)
        requires(is_copy_constructible_v<T1> && is_copy_constructible_v<T2>)
        : first(x), second(y) {}

    template <typename U1, typename U2>
        requires(is_constructible_v<T1, U1> && is_constructible_v<T2, U2>)
    constexpr explicit(!is_convertible_v<U1, T1> || !is_convertible_v<U2, T2>)  //
        pair(U1&& x, U2&& y)
        : first(mystl::forward<U1>(x)), second(mystl::forward<U2>(y)) {}

    constexpr pair(const pair&) = default;
    constexpr pair(pair&&) = default;

    constexpr pair& operator=(const pair&) = default;
    constexpr pair& operator=(pair&&) = default;

    template <typename U1, typename U2>
        requires(is_constructible_v<T1, const U1&> && is_constructible_v<T2, const U2&>)
    constexpr explicit(!is_convertible_v<const U1&, T1> || !is_convertible_v<const U2&, T2>)
        pair(const pair<U1, U2>& p)
        : first(p.first), second(p.second) {}

    template <typename U1, typename U2>
        requires(is_constructible_v<T1, U1 &&> && is_constructible_v<T2, U2 &&>)
    constexpr explicit(!is_convertible_v<U1&&, T1> || !is_convertible_v<U2&&, T2>)
        pair(pair<U1, U2>&& p)
        : first(mystl::forward<U1>(p.first)), second(mystl::forward<U2>(p.second)) {}

    template <typename U1, typename U2>
        requires(is_assignable_v<T1&, const U1&> && is_assignable_v<T2&, const U2&>)
    constexpr pair& operator=(const pair<U1, U2>& p) {
        first = p.first;
        second = p.second;
        return *this;
    }

    template <typename U1, typename U2>
        requires(is_assignable_v<T1&, U1 &&> && is_assignable_v<T2&, U2 &&>)
    constexpr pair& operator=(pair<U1, U2>&& p) {
        first = mystl::forward<U1>(p.first);
        second = mystl::forward<U2>(p.second);
        return *this;
    }
};

template <typename T1, typename T2, typename U1, typename U2>
constexpr bool operator==(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <typename T1, typename T2, typename U1, typename U2>
constexpr auto operator<=>(const pair<T1, T2>& lhs, const pair<U1, U2>& rhs)
    -> std::common_comparison_category_t<decltype(lhs.first <=> rhs.first),
                                         decltype(lhs.second <=> rhs.second)> {
    if (auto cmp = lhs.first <=> rhs.first; cmp != 0) {
        return cmp;
    }
    return lhs.second <=> rhs.second;
}

template <typename T>
struct tuple_size;

template <typename T1, typename T2>
struct tuple_size<pair<T1, T2>> : integral_constant<size_t, 2> {};

template <typename T1, typename T2>
struct tuple_size<const pair<T1, T2>> : integral_constant<size_t, 2> {};

template <size_t I, typename T>
struct tuple_element;

template <typename T1, typename T2>
struct tuple_element<0, pair<T1, T2>> {
    using type = T1;
};

template <typename T1, typename T2>
struct tuple_element<1, pair<T1, T2>> {
    using type = T2;
};

template <size_t I, typename T>
using tuple_element_t = typename tuple_element<I, T>::type;

template <size_t I, typename T1, typename T2>
struct tuple_element<I, const pair<T1, T2>> {
    using type = add_const_t<tuple_element_t<I, pair<T1, T2>>>;
};

template <size_t I, typename T1, typename T2>
constexpr auto& get(pair<T1, T2>& p) noexcept {
    static_assert(I < 2, "Index out of bounds in pair::get");
    if constexpr (I == 0) {
        return p.first;
    } else {
        return p.second;
    }
}

template <size_t I, typename T1, typename T2>
constexpr const auto& get(const pair<T1, T2>& p) noexcept {
    static_assert(I < 2, "Index out of bounds in pair::get");
    if constexpr (I == 0) {
        return p.first;
    } else {
        return p.second;
    }
}

template <size_t I, typename T1, typename T2>
constexpr auto&& get(pair<T1, T2>&& p) noexcept {
    static_assert(I < 2, "Index out of bounds in pair::get");
    if constexpr (I == 0) {
        return mystl::move(p.first);
    } else {
        return mystl::move(p.second);
    }
}

template <size_t I, typename T1, typename T2>
constexpr const auto&& get(const pair<T1, T2>&& p) noexcept {
    static_assert(I < 2, "Index out of bounds in pair::get");
    if constexpr (I == 0) {
        return mystl::move(p.first);
    } else {
        return mystl::move(p.second);
    }
}

template <typename T, typename T1, typename T2>
constexpr T& get(pair<T1, T2>& p) noexcept {
    static_assert(is_same_v<T, T1> ^ is_same_v<T, T2>, "Type T must occur exactly once in pair");
    if constexpr (is_same_v<T, T1>) {
        return p.first;
    } else {
        return p.second;
    }
}

template <typename T, typename T1, typename T2>
constexpr const T& get(const pair<T1, T2>& p) noexcept {
    static_assert(is_same_v<T, T1> ^ is_same_v<T, T2>, "Type T must occur exactly once in pair");
    if constexpr (is_same_v<T, T1>) {
        return p.first;
    } else {
        return p.second;
    }
}

template <typename T, typename T1, typename T2>
constexpr T&& get(pair<T1, T2>&& p) noexcept {
    static_assert(is_same_v<T, T1> ^ is_same_v<T, T2>, "Type T must occur exactly once in pair");
    if constexpr (is_same_v<T, T1>) {
        return mystl::move(p.first);
    } else {
        return mystl::move(p.second);
    }
}

template <typename T, typename T1, typename T2>
constexpr const T&& get(const pair<T1, T2>&& p) noexcept {
    static_assert(is_same_v<T, T1> ^ is_same_v<T, T2>, "Type T must occur exactly once in pair");
    if constexpr (is_same_v<T, T1>) {
        return mystl::move(p.first);
    } else {
        return mystl::move(p.second);
    }
}

template <typename T1, typename T2>
constexpr auto make_pair(T1&& x, T2&& y) {
    using R = pair<unwrap_ref_decay_t<T1>, unwrap_ref_decay_t<T2>>;
    return R(mystl::forward<T1>(x), mystl::forward<T2>(y));
}

}  // namespace mystl

#endif