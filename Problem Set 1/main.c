#include "Everything.h"


#define MAX_config_garb_LENGTH 1000

void* main_thread_func(void*);
void* thread_func_range(void*);
void* thread_func_linear(void*);
void print_all(unsigned long long*, unsigned long long);

typedef struct {
    FILE *config_file;
    int threads, print, task_div;
    unsigned long long input;
} program_config;

typedef struct {
    int thread_id;
    int print_mode;

    unsigned long long num;
    unsigned long long part_lo;
    unsigned long long part_hi;

    unsigned long long* primes;
    unsigned long long primes_count;
} thread_data;

int main() {
    char config_garb[MAX_config_garb_LENGTH];
    program_config config;


    config.config_file = fopen("config.txt", "r");
    if (config.config_file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    while (fgets(config_garb, sizeof(config_garb), config.config_file)) {
        config_garb[strcspn(config_garb, "\n")] = '\0';

        if (strstr(config_garb, "threads = ")) {
            sscanf(config_garb, "threads = %d", &config.threads);
        } else if (strstr(config_garb, "num = ")) {
            sscanf(config_garb, "num = %d", &config.input);
        } else if (strstr(config_garb, "print = ")) {
            sscanf(config_garb, "print = %d", &config.print);
        } else if (strstr(config_garb, "task_div = ")) {
            sscanf(config_garb, "task_div = %d", &config.task_div);
        }
    }

    if (
        (config.threads <= 0) ||
        (config.print != 1 && config.print != 2) ||
        (config.task_div != 1 && config.task_div != 2)
    ) {
        printf("Error in config values! Check the README for instructions.");     
        return EXIT_FAILURE;
    }

    if (config.threads > floor(sqrt(config.input))) {
        printf("Too many threads!");
        return EXIT_FAILURE;
    }

    fclose(config.config_file);

    pthread_t main_thread;

    printf("Listing all primes from 0 to %llu\n", config.input);
    printf("========================================\n\n");

    pthread_create(&main_thread, NULL, main_thread_func, (void*)&config);
    pthread_join(main_thread, 0);
    
    return 0;
}

void *main_thread_func(void* arg) {
    program_config* config = (program_config*) arg;
    pthread_t thread[config->threads];

    if (config->task_div == 1) {
        thread_data* data_arr = malloc(config->threads * sizeof(thread_data));

        unsigned long long part_size = (config->input + (unsigned long long) config->threads - 1) / (unsigned long long) config->threads;  // range size
        data_arr[0].part_lo = 2;
        data_arr[0].part_hi = data_arr[0].part_lo + part_size - 1;

        if (config->print == 2) {
            printf("== Joint Thread ==\n");
        }

        for (int i = 0; i < config->threads; i++) {
            data_arr[i].primes = malloc(part_size * sizeof(unsigned long long));
            data_arr[i].primes_count = 0;


            // prevent overshooting
            if (data_arr[i].part_hi > config->input) {
                data_arr[i].part_hi = config->input;
            }

            data_arr[i].thread_id = i + 1; 
            data_arr[i].num = config->input;
            data_arr[i].print_mode = config->print;

            pthread_create(&thread[i], NULL, thread_func_range, (void*) &data_arr[i]);

            for (unsigned long long j = 0; j < data_arr[i].primes_count; j++) {
                printf("%llu is prime\n", data_arr[i].primes[j]);
            }

            if (data_arr[i].part_hi == config->input) {
                break;
            }

            if (i+1 != config->threads) {
                data_arr[i+1].part_lo = data_arr[i].part_hi + 1;
                data_arr[i+1].part_hi = data_arr[i+1].part_lo + part_size - 1;
            }
        }

        for (int i = 0; i < config->threads; i++) {
            pthread_join(thread[i], 0);
        }

        // unsigned long long total_primes_count = 0;
        // for (int i = 0; i < config->threads; i++) {
        //     total_primes_count += data_arr[i].primes_count;
        // }
        // unsigned long long* all_primes = malloc(total_primes_count * sizeof(unsigned long long));
        // unsigned long long index = 0;
        // for (int i = 0; i < config->threads; i++) {
        //     for (int j = 0; j < data_arr[i].primes_count; j++) {
        //         printf("This is prime: %llu", data_arr[i].primes[j]);
        //         all_primes[index++] = data_arr[i].primes[j];
        //     }
        //     free(data_arr[i].primes);
        // }
        // printf("Total count: %llu", index);

        free(data_arr);
    }
    else {
        pthread_t* thread = malloc((config->input - 2) * sizeof(pthread_t));

        if (config->print == 2) {
            printf("== Joint Thread ==\n");
        }
    
        for (int i = 2; i <= config->input; i++) {
            thread_data* td_copy = malloc(sizeof(thread_data));
            td_copy->print_mode = config->print;
            td_copy->num = i;
            td_copy->thread_id = i - 1;
            pthread_create(&thread[i - 2], NULL, thread_func_linear, (void*)td_copy);
        }
    
        for (int i = 2; i <= config->input; i++) {
            pthread_join(thread[i - 2], NULL);
        }
    
        free(thread);
    }
    return NULL;
}

void* thread_func_range(void* arg) {
    thread_data* td = (thread_data*) arg;
    return getPrimesList(td->thread_id, td->num, td->part_lo, td->part_hi, td->print_mode, td->primes, &(td->primes_count));
}

void* thread_func_linear(void* arg) {
    thread_data* td = (thread_data*) arg;
    isPrime(td->num, td->print_mode, td->thread_id);
    
    return NULL;
}


// void print_all(thread_data* primes) {
//     time_t curr_time = time(NULL);
//     struct tm *time_info = localtime(&curr_time);
//     char time_string[80];
//     strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&curr_time));
//     printf("== Joint Thread | Printing started at %s == \n", time_string);

//     for (unsigned long long i; i < 10; i++) {
//         printf("%llu\n", primes[i]);
//     }
// }

// gcc -o main main.c DivTest.c