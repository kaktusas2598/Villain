#!/bin/bash
rm -rf tests/out/*
cmake -S . -B tests/out/ -G"Unix Makefiles" -DVILLAIN_BUILD_TESTS=ON -DVILLAIN_CODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
make unit_tests -C tests/out/
#make coverage -C tests/out/
