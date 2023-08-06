#!/bin/bash

git submodule update --init --recursive
rm -rf out/*


OPTIND=1 # Reset for previous uses of getops in shell
OPTSTRING="rdb"
BUILD_TYPE="RelWithDebInfo"
BUILD=OFF

# Parse arguments
while getopts "$OPTSTRING" opt; do
    case "$opt" in
    r)
        BUILD_TYPE="Release"
        ;;
    d)
        BUILD_TYPE="Debug"
        ;;
    b)
        BUILD=ON
        ;;
    *)
        echo "Invalid option: $1" >&2
        exit 1
        ;;
    esac
done

cmake -S . -B out/ -G"Unix Makefiles" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"
#cmake -S . -B out/ -G"Visual Studio 15 2017" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}"

if [ "$BUILD" = "ON" ]; then
    make -C out/
fi
