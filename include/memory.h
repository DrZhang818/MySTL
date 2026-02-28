#ifndef MYSTL_HANDMADE_MEMORY_H_
#define MYSTL_HANDMADE_MEMORY_H_

#include <cstddef>
#include "type_traits.h"
#include "utility.h"

namespace mystl {

template <typename T>
constexpr T* addressof(T& arg) noexcept {
#if defined(__GNUEC__) || defined(__clang__) 
    return __builtin_addressof(arg);
#else
    return reinterpret_cast<T*>(
        &const_cast<char&>(reinterpret_cast<const volatile char&>(arg)));
#endif
}

template <typename T>
const T* addressof(const T&&) = delete;

namespace detail {
    template <typename Ptr, bool = requires { typename Ptr::element_type; }>
    struct get_elem_type {};

    template <typename Ptr>
    struct get_elem_type<Ptr, true> {
        using type = typename Ptr::element_type;
    };

    template <template <typename, typename...> class Template, typename T, typename... Args>
    struct get_elem_type<Template<T, Args...>, false> {
        using type = T;
    };

    template <typename Ptr, bool = requires { typename Ptr::element_type; }>
    struct get_diff_type {
        using type = std::ptrdiff_t;
    };

    template <typename Ptr>
    struct get_diff_type<Ptr, true> {   
        using type = typename Ptr::difference_type;
    };

    template <typename Ptr, typename U, bool = requires { typename Ptr::template rebind<U>; }>
    struct get_rebind {};

    template <typename Ptr, typename U>
    struct get_rebind<Ptr, U, true> {
        using type = typename Ptr::template rebind<U>;
    };

    template <template <typename, typename...> class Template, typename T, typename... Args, typename U>
    struct get_rebind<Template<T, Args...>, U, false> {
        using type = Template<U, Args...>;
    };
}

template <typename Ptr>
struct pointer_traits {
    using pointer           = Ptr;
    using element_type      = typename detail::get_elem_type<Ptr>::type;
    using different_type    = typename detail::get_diff_type<Ptr>::type;

    template <typename U>
    using rebind = typename detail::get_rebind<Ptr, U>::type;

    static constexpr pointer pointer_to(element_type& r) noexcept {
        return Ptr::pointer_to(r);
    }
};

template <typename T>
struct pointer_traits<T*> {
    using pointer = T*;
    using element_type = T;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    using rebind = U*;

    static constexpr pointer pointer_to(element_type& r) noexcept {
        return mystl::addressof(r);
    }
};

}

#endif