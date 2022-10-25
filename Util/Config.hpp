#pragma once

#include <type_traits>

#define ESSA_ALWAYS_INLINE [[gnu::always_inline]]
#define ESSA_UNREACHABLE __builtin_unreachable()

namespace Util {

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };

template<typename... Deps>
inline constexpr bool DependentFalse = false;

template<class T>
concept Enum = std::is_enum_v<T>;

namespace Detail {

template<class T, class... Ts>
struct _First { using Type = T; };

}

template<class... Ts>
using First = typename Detail::_First<Ts...>::Type;

}
