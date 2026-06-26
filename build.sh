#!/bin/bash
mkdir -p bin
g++ Encoder.cpp -o bin/encoder
g++ Decoder.cpp -o bin/decoder