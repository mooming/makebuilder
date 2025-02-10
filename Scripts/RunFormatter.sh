#!/bin/sh
find ../Common/  -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i
find ../MakeBuild/ -iname '*.h' -o -iname '*.cpp' | xargs clang-format -i
