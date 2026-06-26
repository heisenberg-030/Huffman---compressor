#!/bin/bash
mkdir -p bin
g++ encoder.cpp -o bin/encoder
g++ decoder.cpp -o bin/decoder