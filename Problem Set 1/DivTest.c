#include "Everything.h"

unsigned long long* getPrimesList(int id, unsigned long long num, unsigned long long lwr_bnd, unsigned long long upp_bnd, int print, unsigned long long* list, unsigned long long* p_count) {
    unsigned long long count = 0;
    unsigned long long* prime_list = malloc((upp_bnd - lwr_bnd + 2) * sizeof(unsigned long long));

    for (unsigned long long i = lwr_bnd; i <= upp_bnd; i++) {

        if (isPrime(i, print, id)) {
            prime_list[count++] = i;
        }
    }
    // printf("Count: %i\n", count);
    *p_count = count;
    list = prime_list;
    printf("\n===== Thread %d done | Primes found by thread: %d =====\n\n", id, count);

    return prime_list;
}

bool isPrime(unsigned long long num, int print, int id) {
    time_t curr_time = time(NULL);
    struct tm *time_info = localtime(&curr_time);
    char time_string[80];
    for (unsigned long long i = 2; i < num; i++) {
        if (isDivisible(num, i)) {
            return false;
        }
    }

    if (print == 1) {
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&curr_time));
        printf("%llu is prime | Thread ID: %d | Timestamp: %s\n", num, id, time_string);
    }
    
    return true;
}

bool isDivisible(unsigned long long num, unsigned long long div) {
    return (num % div == 0);
}

// gcc -o main main.c DivTest.c