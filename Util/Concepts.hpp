#pragma once

namespace llgl::concepts
{

template<class T>
concept Vector3 = requires(T t, T u) { t.x; t.y; t.z; T{t.x, t.y, t.z}; };

template<class T>
concept Vector2 = !Vector3<T> && requires(T t, T u) { t.x; t.y; T{t.x, t.y}; };

}
