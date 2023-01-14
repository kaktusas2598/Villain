#!/bin/bash

git submodule add https://github.com/g-truc/glm.git vendor/glm

rm -rf build/*
cmake -S . -B build/ -G"Unix Makefiles"
make -C build/
cp -R assets build/
