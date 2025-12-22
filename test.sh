#!/bin/sh

clear
gcc -lm ftext.c -o bin/ftext &&\
        ./bin/ftext < tests/input.txt
