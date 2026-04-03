# Agent Guidelines for MakeBuilder

This is a C++23 CMake-based meta-build system that automatically generates CMakeLists.txt files based on project directory structure.

## Build Commands

### Configure and Build
```bash
cmake -B <build_dir> -S .
cmake --build <build_dir>
```

### Run the Tool
```bash
./bin/mbuild <projects_root_path>
```

### Format Code
```bash
./Scripts/RunFormatter.sh  # Runs clang-format on all .h and .cpp files
```

### Manual Format
```bash
clang-format -i <file.cpp>   # Format single file in-place
```

### Run clang-tidy
```bash
clang-tidy <file.cpp> -- -std=c++23
```

### Clean Build
```bash
rm -rf <build_dir>
cmake -B <build_dir> -S .
cmake --build <build_dir>
```

## Code Style

### General Rules
- C++23 standard
- 4 spaces indentation, no tabs
- 120 column limit
- `#pragma once` for header guards
- LLVM-based formatting (see `.clang-format`)

### Braces
```cpp
// Function definitions - brace on next line (K&R style)
void MyFunction()
{
    if (condition)
    {
        // ...
    }
}

// Control statements - space before paren
if (condition)
{
}

// Classes - break after opening brace
class MyClass
{
public:
    // ...
private:
    // ...
};
```

### Namespaces
- Main app namespace: `mb`
- OS abstraction: `OS`
- Utilities: `Util`
- No indentation within namespace

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Classes | PascalCase | `ProjectBuilder`, `BuildConfig` |
| Enums | PascalCase with `E` prefix | `EBuildType` |
| Enum values | PascalCase | `EBuildType::Executable` |
| Functions | PascalCase | `BuildCMakeFiles()` |
| Variables | camelCase | `moduleName`, `buildType` |
| Type aliases | PascalCase with suffix | `TString`, `Modules` |
| Private members | Simple names, no underscore | `config` |

### Type Aliases Pattern
```cpp
class Module {
    using TString = std::string;
    using TSize = size_t;
    using Modules = std::vector<Module>;
    using Strings = std::vector<TString>;
};
```

### Enums Pattern
```cpp
enum class EBuildType : uint8_t {
    None = 0,
    Ignored,
    Executable,
    StaticLibrary,
    SharedLibrary,
};
```

### Access Modifiers
- Order: `public`, `protected`, `private`
- No indentation within class body
- Logical block separation between groups

## Imports / Includes

Include order:
1. Project headers (`"..."`) - priority 1
2. LLVM/Clang headers (`<"llvm/..."`) - priority 2
3. External headers (`<gtest/...`, `<json/...>`) - priority 3
4. Standard library (`<...>`)

```cpp
#include "BuildConfig.h"
#include "Directory.h"
#include <vector>
#include <string>
#include <memory>
```

## Error Handling

- `assert()` for null checks and invariants
- `abort()` for unrecoverable errors
- Console output: `cout << "[Module] message"`
- Early returns for error conditions
- `std::optional<T>` for potentially missing config values

```cpp
if (config == nullptr)
{
    abort();
}

if (!IsValid(path))
    return false;
```

## Cross-Platform

Platform detection via preprocessor:
```cpp
#ifdef WINDOWS
    // Windows-specific code
#elif defined(POSIX)
    // POSIX-compliant code
#endif
```

Use `OSAbstractLayer` for cross-platform operations:
- `GetFullPath()` - absolute path
- `IsDirectory()` - check directory
- `ListFilesInDirectory()` - list contents

## Module System

### Module Types (from `.module.config`)
- `Ignored` - Skip this module
- `HeaderOnly` - Header-only library
- `Executable` - Produces executable
- `StaticLibrary` - Static library (.a/.lib)
- `SharedLibrary` - Shared library (.so/.dll)
- `ExternalLibrary` - External library directory
- `ExternalCMakeProject` - External CMake project

### Module Specifier Files
- `include.txt` - Directory is an include path
- `dependency.txt` - Module dependencies
- `library.txt` - Required libraries
- `framework.txt` - Required frameworks (macOS)

## File Structure

```
Application/MakeBuild/  - Main application logic
Source/Common/          - Shared library utilities
```

## Testing

This project has no test suite. Do not add tests unless explicitly requested.

## Linting

Run `clang-tidy` checks manually on modified files before committing. The `.clang-tidy` file enables:
- bugprone-* checks
- cert-* checks  
- cppcoreguidelines-* checks
- modernize-* checks
- performance-* checks
- readability-* checks
