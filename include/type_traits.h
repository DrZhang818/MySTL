#ifndef MYSTL_HADEMADE_TYPE_TRAITS_H_
#define MYSTL_HADEMADE_TYPE_TRAITS_H_

#include <cstddef>

namespace mystl {

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant;

    constexpr operator value_type() const noexcept   { return value; }

    constexpr value_type operator()() const noexcept { return value; }
};

template <bool b>
using bool_constant = integral_constant<bool, b>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <bool B, typename T = void> struct enable_if {};
template <typename T> struct enable_if<true, T>       { using type = T; };

template <bool B, typename T = void>
using enable_if_t = typename enable_if<B, T>::type;

template <bool B, typename T, typename F> struct conditional      { using type = T; };
template <typename T, typename F> struct conditional<false, T, F> { using type = F; };

template <bool B, typename T, typename F>
using conditional_t = typename conditional<B, T, F>::type;

template <typename T> struct remove_reference       { using type = T; };
template <typename T> struct remove_reference<T&>   { using type = T; };
template <typename T> struct remove_reference<T&&>  { using type = T; };

template <typename T>
using remove_reference_t = typename remove_reference<T>::type;

template <typename T> struct remove_const           { using type = T; };
template <typename T> struct remove_const<const T>  { using type = T; };

template <typename T>
using remove_const_t = typename remove_const<T>::type;

template <typename T> struct remove_volatile              { using type = T; };
template <typename T> struct remove_volatile<volatile T>  { using type = T; };

template <typename T>
using remove_volatile_t = typename remove_volatile<T>::type;

template <typename T>
struct remove_cv {
    using type = typename remove_volatile<typename remove_const<T>::type>::type;
};

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
using remove_cvref_t = typename remove_cv<typename remove_reference<T>::type>::type;

template <typename T> struct is_pointer_helper          : false_type {};
template <typename T> struct is_pointer_helper<T*>      : true_type {};

template <typename T>
struct is_pointer : is_pointer_helper<remove_cv_t<T>> {};

template <typename T> struct is_lvalue_reference        : false_type {};
template <typename T> struct is_lvalue_reference<T&>    : true_type {};

template <typename T> struct is_rvalue_reference        : false_type {};
template <typename T> struct is_rvalue_reference<T&&>   : true_type {};


template <typename T>
constexpr T&& forward(remove_reference_t<T>& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <typename T>
constexpr T&& forward(remove_reference_t<T>&& arg) noexcept {
    return static_cast<T&&>(arg);
}

template <typename T>
constexpr remove_reference_t<T>&& move(T&& arg) noexcept {
    return static_cast<remove_reference_t<T>&&>(arg);
}

template <typename T, typename U> struct is_same       : false_type {};
template <typename T>             struct is_same<T, T> : true_type {};

template <typename T, typename U> 
inline constexpr bool is_same_v = is_same<T, U>::value;

template <typename T>
inline constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;

template <typename T>
struct is_void : bool_constant<is_void_v<T>> {};

template <typename T>
struct is_trivially_destructible
    : bool_constant<__has_trivial_destructor(T)> {};

template <typename T>
inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

template <typename T1, typename T2>
struct pair;

template <typename T>
struct is_pair : false_type {};

template <typename T1, typename T2>
struct is_pair<pair<T1, T2>> : true_type {};

template <typename T>
inline constexpr bool is_pair_v = is_pair<remove_cv_t<T>>::value;

template <typename T>
concept Pair = is_pair_v<T>;

}

#endif