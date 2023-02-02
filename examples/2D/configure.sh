#!/bin/bash

#git submodule add https://github.com/g-truc/glm.git vendor/glm
git submodule update --init --recursive

rm -rf out/*
# Can specify option with cmake -DOPTION_NAME=VALUE
#cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo
#cmake -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B out/ -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
#-G"Visual Studio 15 2017"
