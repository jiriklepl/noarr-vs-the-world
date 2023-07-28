#!/usr/bin/env bash

cmake -E make_directory build || exit 1
cmake -E chdir build cmake -DCMAKE_BUILD_TYPE=Release .. || exit 1
cmake -E chdir build cmake --build . --config Release || exit 1
