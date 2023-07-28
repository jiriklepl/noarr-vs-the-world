#!/usr/bin/env bash

cmake -E make_directory build || exit 1
cmake -E chdir build cmake \
    -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-DLARGE_DATASET -DDATA_TYPE_IS_DOUBLE " \
    .. || exit 1

NPROC=$(nproc)
cmake -E chdir build cmake --build . --config Release -j$(NPROC) || exit 1
