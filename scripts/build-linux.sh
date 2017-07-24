#!/bin/bash
cd "$(dirname "$0")/.."

g++ src/bitp.cpp -o dist/bitp-linux
