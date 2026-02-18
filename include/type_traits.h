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

template <typename...> 
using void_t = void;

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

namespace detail {
    template <typename T> struct is_pointer_helper          : false_type {};
    template <typename T> struct is_pointer_helper<T*>      : true_type {};
}

template <typename T>
struct is_pointer : detail::is_pointer_helper<remove_cv_t<T>> {};

template <typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

template <typename T> struct is_lvalue_reference        : false_type {};
template <typename T> struct is_lvalue_reference<T&>    : true_type {};

template <typename T> struct is_rvalue_reference        : false_type {};
template <typename T> struct is_rvalue_reference<T&&>   : true_type {};

template <typename T> 
inline constexpr bool is_lvalue_reference_v = is_lvalue_reference<T>::value;

template <typename T>
inline constexpr bool is_rvalue_reference_v = is_rvalue_reference<T>::value;

template <typename T> struct is_reference               : false_type {};
template <typename T> struct is_reference<T&>           : true_type {};
template <typename T> struct is_reference<T&&>          : true_type {};

template <typename T>
inline constexpr bool is_reference_v = is_reference<T>::value;

template <typename T>
concept Reference = is_reference_v<T>;

template <typename T> struct add_pointer { using type = remove_reference_t<T>*; };

template <typename T>
using add_pointer_t = typename add_pointer<T>::type;

template <typename T, typename = void> struct add_lvalue_reference { using type = T; };
template <typename T> struct add_lvalue_reference<T, void_t<T&>>   { using type = T&; };

template <typename T>
using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

template <typename T, typename = void> struct add_rvalue_reference { using type = T; };
template <typename T> struct add_rvalue_reference<T, void_t<T&&>>  { using type = T&&; };

template <typename T>
using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

template <typename T>
add_rvalue_reference_t<T> declval() noexcept;

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

template <typename T> struct remove_extent                  { using type = T; };
template <typename T> struct remove_extent<T[]>             { using type = T; };
template <typename T, size_t N> struct remove_extent<T[N]>  { using type = T; };

template <typename T>
using remove_extent_t = typename remove_extent<T>::type;

template <typename T, typename U> struct is_same       : false_type {};
template <typename T>             struct is_same<T, T> : true_type {};

template <typename T, typename U> 
inline constexpr bool is_same_v = is_same<T, U>::value;

template <typename T>
inline constexpr bool is_void_v = is_same_v<remove_cv_t<T>, void>;

template <typename T>
struct is_void : bool_constant<is_void_v<T>> {};

template <typename T> struct is_const                  : false_type {};
template <typename T> struct is_const<const T>         : true_type {};

template <typename T>
inline constexpr bool is_const_v = is_const<T>::value;

template <typename T> struct is_volatile               : false_type {};
template <typename T> struct is_volatile<volatile T>   : true_type {};

template <typename T>
inline constexpr bool is_volatile_v = is_volatile<T>::value;

template <typename T>
struct is_trivially_destructible
    : bool_constant<__has_trivial_destructor(T)> {};

template <typename T>
inline constexpr bool is_trivially_destructible_v = is_trivially_destructible<T>::value;

template <typename T> struct is_array                  : false_type {};
template <typename T> struct is_array<T[]>             : true_type {};  
template <typename T, size_t N> struct is_array<T[N]>  : true_type {};

template <typename T>
inline constexpr bool is_array_v = is_array<T>::value;

template <typename T> struct is_function : bool_constant<!is_const_v<const T> && !is_reference_v<T>> {};

template <typename T>
inline constexpr bool is_function_v = is_function<T>::value;

namespace detail {
    template <typename U> struct decay_selector { using type = conditional_t<is_function_v<U>, add_pointer_t<U>, remove_cv_t<U>>; };
    template <typename U, size_t N> struct decay_selector<U[N]> { using type = U*; };
    template <typename U> struct decay_selector<U[]>  { using type = U*; };
}

template <typename T> struct decay { using type = typename detail::decay_selector<remove_reference_t<T>>::type; };

template <typename T>
using decay_t = typename decay<T>::type;

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