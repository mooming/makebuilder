#!/bin/sh
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

echo "Formatting source files..."

# Format Source/Common
find "$PROJECT_ROOT/Source/Common" -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

# Format Application/MakeBuild
find "$PROJECT_ROOT/Application/MakeBuild" -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

# Format test cases
find "$PROJECT_ROOT/TestCases" -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i

echo "Formatting complete."
