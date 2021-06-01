#!/bin/sh
clang++ -lpthread -lrt camera.cpp -o camera.out `pkg-config --cflags --libs opencv`