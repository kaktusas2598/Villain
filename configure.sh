#!/bin/bash

#git submodule add https://github.com/g-truc/glm.git vendor/glm
git submodule update --init --recursive

rm -rf build/*
# Can specify option with cmake -DOPTION_NAME=VALUE
cmake -S . -B build/ -G"Unix Makefiles"
