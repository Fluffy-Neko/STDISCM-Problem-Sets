@echo off

:: Navigate to the target directory
cd "C:\Users\chris\Documents\Term 14\STDISCM\STDISCM-Problem-Sets\Problem Set 2\LFG Dungeon Queueing"

:: Compile the C program
gcc -o main main.c

:: Check if the compiled executable exists and run it
if exist main.exe (
    main.exe
) else (
    main
)