[Config Text File]
1. Set the preferred number of threads to use and the number to find primes lesser than it
>>>> The number of threads should be greater than 0 and less than the floored square root of the input number

2. 'print' and 'task_div' should only have 1 or 2 as its value
>>>> print = 1 | prints Thread id and time stamp immediately; 
>>>> print = 2 | wait until all threads are done then print everything

>>>> task_div = 1 | Straight division of search range
>>>> task_div = 2 | Linear search but the threads are for divisibility testing of individual numbers

3. The maximum number that can be used to find the primes of is ULLONG_MAX = 18_446_744_073_709_551_615



[Instructions]
- Entry point is in main.c
- Use the command gcc -o run main.c <filename_x>.c <filename_y>.c ... to compile the source code
- Run the program
>>>> Invoke `run` if using cmd 
>>>> Use `./run` if powershell/bash