#!/bin/sh
clang++ -lpthread -lrt ascii.cpp -o ascii.out `pkg-config --cflags --libs opencv`