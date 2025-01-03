#pragma once
#include <concepts>
#include <type_traits>

// A collection of concepts I find handy.
template<typename T, typename... Ts>
struct first_arg {
    using type = T;
};

template<typename... Args>
using first_arg_t = typename first_arg<Args...>::type;

template<typename T, size_t count, typename...Args>
concept IsVarArray = requires(T matchType, Args... args) {
    requires std::conjunction_v<std::is_same<T, Args>...>;
    requires sizeof...(args) == count;
};

//template<typename T>
//concept AreAllSame = true;

template<typename T, typename... Rest>
concept AreAllSame = requires {
    requires std::conjunction_v<std::is_same<T, Rest>...>;
};