#!/bin/bash

cd "C:/Users/chris/Documents/Term 14/STDISCM/STDISCM-Problem-Sets/Problem Set 2/LFG Dungeon Queueing"
gcc -o main main.c
if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" ]]; then
    ./main.exe
else
    ./main
fi