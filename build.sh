#!/usr/bin/env bash
# build.sh - Build the makebuild target with optional flags
# Usage: build.sh [-dev] [-debug] [-release] [-clean]
# Example: build.sh -dev -debug -release -clean

set -e

# Default settings
CLEAN=0
TARGET="makebuild"

# Configuration flags (can specify multiple)
CONFIGS=()

# Parse arguments
while [[ $# -gt 0 ]]; do
  case "$1" in
    -dev)
      CONFIGS+=("Dev")
      ;;
    -debug)
      CONFIGS+=("Debug")
      ;;
    -release)
      CONFIGS+=("Release")
      ;;
    -clean)
      CLEAN=1
      ;;
    *)
      echo "Unexpected argument: $1"
      exit 1
      ;;
  esac
  shift
done

# If no explicit config flags were given, default to Dev
if [[ ${#CONFIGS[@]} -eq 0 ]]; then
  CONFIGS+=("Dev")
fi

if [[ -z "$TARGET" ]]; then
  echo "Error: Target not defined."
  exit 1
fi

# Ensure the build directory is generated (CMake configuration)
echo "Generating build files..."
cmake -B build -S . -G "Ninja Multi-Config" --fresh

# Iterate over each requested configuration
for CONFIG in "${CONFIGS[@]}"; do
  # Clean if requested
  if [[ $CLEAN -eq 1 ]]; then
    echo "Cleaning build for $TARGET (config $CONFIG)"
    cmake --build build --config "$CONFIG" --target clean || true
  fi

  # Build the target
  echo "Building target $TARGET with configuration $CONFIG"
  cmake --build build --config "$CONFIG" --target "$TARGET"

done
