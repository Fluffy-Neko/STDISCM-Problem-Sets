#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <locale.h>

#ifndef GET_PRIMES_H
#define GET_PRIMES_H

bool isDivisible(unsigned long long, unsigned long long);
bool isPrime(unsigned long long, int, int);
unsigned long long* getPrimesList(int, unsigned long long, unsigned long long, unsigned long long, int, unsigned long long*, unsigned long long*);

#endif
