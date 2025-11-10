# x_enum
Lightweight zero-cost cross-platform header-only C++ library for introducing reflection capabilities to native C++ enums

## Features
- Only one header, no dependencies
- C++20-compatible
- Cross-platform
- Introduces reflection to existing native С++-enums
- Based on X macro technology, does not make any use of template magic
- Supports any native С++ enums without restrictions on their size

## Installation
Simply copy `x_enum.hpp` into your project or link it via CMake:

```cmake
# CMake
include(FetchContent)
FetchContent_Declare(
  x_enum
  GIT_REPOSITORY https://github.com/mikhailshevtsov/x_enum.git
  GIT_TAG main
)
FetchContent_MakeAvailable(x_enum)
target_link_libraries(your_project PRIVATE x_enum)
```

## Example of usage
```cpp
#include "x_enum.hpp"
#include <iostream>

// Macro description of enum Color
#define X_ENUM_Color(X) \
    X(Red, 42) \
    X(Green) \
    X(Blue)

// Reflective enum inside global namespace
X_ENUM(Color)

// Refelective enum inside namespace
namespace YourNamespace
{
namespace YourNestedNamespace
{
X_ENUM(Color)
}
}

// Reflective enum inside struct or class
struct YourStruct
{
    X_ENUM_IN_CLASS(Color)
};

// Regular enum inside namespace (it could be inside some third-party library)
namespace ExternalNamespace
{
namespace ExternalNestedNamespace
{
enum class Color { Red = 42, Green, Blue };
}
}

// Regular enum inside struct or class 
struct ExternalStruct
{
    enum class Color { Red = 42, Green, Blue };
};

// Adding reflection capabilites to existing enums
X_DESCRIBE_ENUM_EXTERNAL(Color, ExternalNamespace::ExternalNestedNamespace)
X_DESCRIBE_ENUM_EXTERNAL(Color, ExternalStruct)

int main()
{
    using EnumT = Color;
    using ValueT = x_enum::underlying_type_t<EnumT>;

    std::cout << x_enum::enum_name<EnumT>() << " enum has " << x_enum::enum_size<EnumT>() << " enumers:\n";
    for (EnumT enumer : x_enum::enum_enumers<EnumT>())
    {
        std::cout << "  ";
        std::cout << x_enum::to_string(enumer) << " = ";
        std::cout << static_cast<ValueT>(enumer) << "\n";
    }

    return 0;
}
```

## License
MIT License © 2025 Mikhail Shevtsov
