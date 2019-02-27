#!/usr/bin/env bash

cd build
clang-tidy -header-filter="src/*/*" ../src/**/*
