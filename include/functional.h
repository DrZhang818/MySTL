#ifndef MYSTL_HANDMADE_FUNCTIONAL_H_
#define MYSTL_HANDMADE_FUNCTIONAL_H_

#include "type_traits.h"

namespace mystl {

namespace detail {

template <typename C, typename F, typename Obj, typename... Args>
constexpr decltype(auto) invoke_impl(F C::* f, Obj&& obj, Args&&... args)
    requires is_member_function_pointer_v<F C::*>
{
    if constexpr (is_base_of_v<C, decay_t<Obj>>) {
        return (mystl::forward<Obj>(obj).*f)(mystl::forward<Args>(args)...);
    } else {
        return ((*mystl::forward<Obj>(obj)).*f)(mystl::forward<Args>(args)...);
    }
}

template <typename C, typename M, typename Obj>
constexpr decltype(auto) invoke_impl(M C::* m, Obj&& obj)
    requires is_member_object_pointer_v<M C::*>
{
    if constexpr (is_base_of_v<C, decay_t<Obj>>) {
        return mystl::forward<Obj>(obj).*m;
    } else {
        return (*mystl::forward<Obj>(obj)).*m;
    }
}

template <typename F, typename... Args>
constexpr decltype(auto) invoke_impl(F&& f, Args&&... args)
    requires(!is_member_pointer_v<decay_t<F>>)
{
    return mystl::forward<F>(f)(mystl::forward<Args>(args)...);
}

}  // namespace detail

template <typename F, typename... Args>
constexpr decltype(auto) invoke(F&& f, Args&&... args) {
    return detail::invoke_impl(mystl::forward<F>(f), mystl::forward<Args>(args)...);
}

}  // namespace mystl

#endif