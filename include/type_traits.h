#pragma once

#include <type_traits>

namespace mystl {
    template <typename T, T v>
    using m_integral_constant = std::integral_constant<T, v>;

    template <bool b>
    using m_bool_constant = m_integral_constant<bool, b>;

    using m_true_type = m_bool_constant<true>;
    using m_false_type = m_bool_constant<false>;

    template <typename T1, typename T2>
    struct pair;

    template <typename T>
    struct is_pair : m_false_type {};

    template <typename T1, typename T2>
    struct is_pair<mystl::pair<T1, T2>> : m_true_type {};

    template <typename T>
    inline constexpr bool is_pair_v = is_pair<std::remove_cv_t<T>>::value;

    template <typename T>
    concept Pair = is_pair_v<T>;
}

