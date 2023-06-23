#!/bin/bash
rm -rf out/*
cmake -S . -B out/ -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo
