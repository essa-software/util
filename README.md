# ESSA Util

**ESSA Util** is a general-purpose utility library, a supplement to C++ standard library.

## Full feature list

* Custom Unicode-aware [strings](./Util/UString.hpp) and [streams](./Util/Stream/) (WIP)
* Custom 2,3,4-dimensional [vectors](./Util/Vector.hpp), [matrices](./Util/Matrix.hpp), [rects](./Util/Rect.hpp)
* [Colors](./Util/Color.hpp): RGB and HSV support, many predefined colors
* Utility classes ([`NonCopyable`](./Util/NonCopyable.hpp), [`DelayedInit`](./Util/DelayedInit.hpp))
* [Unit wrappers](./Util/Units.hpp) ([angle](./Util/Angle.hpp), length, distance etc) and [unit formatting](./Util/UnitDisplay.hpp)
* [Clock utilities](./Util/SimulationClock.hpp) and formatters (C++-y wrapper for `strftime`)
* [Endianness](./Util/Endianness.hpp) conversions
* [Character type](./Util/CharacterType.hpp) detection (wrapper for C functions like `isdigit`, `isalpha`)
* Some things imported from [SerenityOS AK](https://github.com/SerenityOS/serenity/tree/master/AK):
    * [`Is.hpp`](./Util/Is.hpp) - dynamic & static polymorphism helpers
    * [`Error.hpp`](./Util/Error.hpp) - `TRY()` error handling
    * [`Enum.hpp`](./Util/Enum.hpp) - bitfield operators for enums
* CMake function `essautil_setup_target` to setup sensible defaults for targets (C++20, all warnings treated as errors, sanitizers enabled in Debug and optimizations in Release, add project root directory to include path)
* (WIP) [Testing framework](./Tests) - currently not exported

## Building

Required dependencies: `fmt`

Simple install (will use global system-wide location):
```sh
./install.sh
```

If you want to run cmake manually:
```sh
mkdir build
cd build
cmake .. -GNinja
ninja
sudo ninja install
```
