#pragma once

#define ESSA_ALWAYS_INLINE [[gnu::always_inline]]
#define ESSA_UNREACHABLE __builtin_unreachable()

namespace Util {

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };

template<typename... Deps>
inline constexpr bool DependentFalse = false;

}
