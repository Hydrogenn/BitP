#!/bin/bash
cd "$(dirname "$0")/.." || exit 1

clang++ -std=c++11 -stdlib=libc++ src/bitp.cpp -o dist/bitp-mac