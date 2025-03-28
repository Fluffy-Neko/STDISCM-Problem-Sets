#include "misc.h"

double* total_duration;
int* total_parties_served;

struct {
    // Head is the pointer
    int tank_q_head;
    int tank_q_tail;

    int dps_q_head;
    int dps_q_tail;

    int heal_q_head;
    int heal_q_tail;
} data_q;
pthread_mutex_t data_q_lock; // Mutex for protecting data_q
sem_t dungeon_semaphore; // Semaphore to limit the number of concurrent threads

typedef struct {
    int id;
    int duration;
} dungeon_instance;

typedef struct {
    int n;
    int tank_n;
    int dps_n;
    int heal_n;
    int t1;
    int t2;
} input_attr;

void queue_init();
int get_totalmax_instances(int, int, int);
void dungeon_init(dungeon_instance*, unsigned int, int, int);
void* dungeon_start(void*);
int get_random_time(int, int, unsigned int*);
int* get_remaining_players(int);
void display_loading(char*, char*);
void display_results(input_attr);


int main(int argc, char* argv[]) {
    FILE* input_file;
    char input_text[1000];
    input_attr input = {0, 0, 0, 0, 0, 0}; 
    input_file = fopen("input.txt", "r");

    if (input_file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    while (fgets(input_text, sizeof(input_text), input_file)) {
        input_text[strcspn(input_text, "\n")] = '\0';

        if (strstr(input_text, "n = ")) {
            // printf("1\n");
            sscanf(input_text, "n = %d", &input.n);
        } else if (strstr(input_text, "tank = ")) {
            // printf("2\n");
            sscanf(input_text, "tank = %d", &input.tank_n);
        } else if (strstr(input_text, "dps = ")) {
            // printf("3\n");
            sscanf(input_text, "dps = %d", &input.dps_n);
        } else if (strstr(input_text, "heal = ")) {
            // printf("4\n");
            sscanf(input_text, "heal = %d", &input.heal_n);
        } else if (strstr(input_text, "t1 = ")) {
            // printf("5\n");
            sscanf(input_text, "t1 = %d", &input.t1);
        } else if (strstr(input_text, "t2 = ")) {
            // printf("6\n");
            sscanf(input_text, "t2 = %d", &input.t2);
        } else {
            printf("Error in input values! Check the README for instructions.");     
            return EXIT_FAILURE;
        }
        // printf("1 line done\n");
    }

    if (true) {
        // ---------------------------------------------------------------------------------------------------------
        if (input.t1 < 1 || input.t2 < 1) {
            fprintf(stderr, "Error: Dungeon instance start/finish time must be positive\n");
            return EXIT_FAILURE;
        }
    
        if (input.t2 < input.t1) {
            fprintf(stderr, "Error: Dungeon instance start time must be less than finish time\n");
            return EXIT_FAILURE;
        }
    
        if (input.tank_n < 1 || input.dps_n < 1 || input.heal_n < 1) {
            fprintf(stderr, "Queueing Error: There must be at least 1 player for each role\n");
            printf("Current number of players:\n");
            printf("Tank: %d\n", input.tank_n);
            printf("DPS: %d\n", input.dps_n);
            printf("Healer: %d\n", input.heal_n); 
            return EXIT_FAILURE;
        }
    
        if (input.n < 1) {
            fprintf(stderr, "Error: There must be at least 1 dungeon instance that can run at a time\n");
            return EXIT_FAILURE;
        }
    
        
        // printf("n = %d\n", input.n);
        // printf("tank_n = %d\n", input.tank_n);
        // printf("dps_n = %d\n", input.dps_n);
        // printf("heal_n = %d\n", input.heal_n); 
        // printf("t1 = %d\n", input.t1);
        // printf("t2 = %d\n", input.t2);

        // ---------------------------------------------------------------------------------------------------------
    }

    queue_init(input.tank_n, input.dps_n, input.heal_n);
    display_loading("Creating Dungeon Instances", "Dungeon Instances Created!");
    dungeon_instance instances[input.n];

    pthread_t threads[input.n];
    pthread_mutex_init(&data_q_lock, NULL);
    sem_init(&dungeon_semaphore, 0, input.n);

    total_parties_served = calloc(input.n, sizeof(int));
    total_duration = calloc(input.n, sizeof(double));

    // for (int i = 0; i < 10; i++) {
    //     unsigned int seed = i + time(NULL);
    //     printf("Random number: %d\n", get_random_time(input.t1, input.t2, &(seed)));
    // }

    // Whatever this is
    // I did not get why there are always extra instances
    // I tried to implement a condition that uses the function get_totalmax_instances, though it's too much of a brute force and
    // assumes the program can see the future and know the total number of instances that will be created
    // This might be because when one threads is in the process of incrementing the head, another thread might be checking the condition
    // which means a race condition happens and an extra instance is created; this can be seen if you compare the total instances
    // if n = 1 vs. n > 1
    // This implementation is the only one that I could think of, but whatever, it works

    // I then realized that checking the heads and tails is useless, because given the fact that the extra number of players 
    // will always be dropped, the total number of instances can always be inferred from the minimum number of players in each role
    // Why did I not just think if this reasoning before?
    int dungeon_id = 0; // Circular index for dungeon IDs
    int x = 0;
    printf("\n");
    while (
            // data_q.tank_q_head <= data_q.tank_q_tail &&
            // data_q.dps_q_head <= data_q.dps_q_tail-2 &&
            // data_q.heal_q_head <= data_q.heal_q_tail
            // && 
            x < get_totalmax_instances(input.tank_n, input.dps_n, input.heal_n)
        ) {
        unsigned int seed = dungeon_id + time(NULL);
        dungeon_init(&instances[dungeon_id], dungeon_id, input.t1, input.t2);

        int* thread_id = malloc(sizeof(int));
        *thread_id = dungeon_id;
        sem_wait(&dungeon_semaphore);

        // time_t start_time = time(NULL);
        pthread_create(&threads[dungeon_id], NULL, dungeon_start, (void*)&instances[dungeon_id]);
        // time_t end_time = time(NULL);

        dungeon_id = ((dungeon_id + 1) % input.n);
        total_parties_served[dungeon_id]++;
        // total_duration[dungeon_id] += difftime(end_time, start_time);
        x++;
    }

    for (int i = 0; i < input.n; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&data_q_lock);
    sem_destroy(&dungeon_semaphore);

    // printf("\n\n\n x == %d\n", x);

    // I feel like this is more of a brute force approach
    // There is a dungeon id var independent of the dungeon_id element of the dungeon_instance struct
    // Howeverm, I could not find a more logical way to implement initializing all
    // dungeons first, then looping only on those to start the threads
    // whatever, it works; I'm happy with it for now :) 


    display_results(input);

    return 0;
}

// Simulate the queueing of players
void queue_init(int tank_n, int dps_n, int heal_n) {
    // Could use a for loop for a bit more realism
    // but this implementation is much simpler
    // just imagine players being queued in this function

    display_loading("Queueing Tank Players", "Tank Players Queued!");
    data_q.tank_q_head = 1;
    data_q.tank_q_tail = tank_n;

    display_loading("Queueing DPS Players", "DPS Players Queued!");
    data_q.dps_q_head = 1;
    data_q.dps_q_tail = dps_n;

    display_loading("Queueing Healer Players", "Healer Players Queued!");
    data_q.heal_q_head = 1;
    data_q.heal_q_tail = heal_n;

}

void dungeon_init(dungeon_instance* d, unsigned int i, int t1, int t2) {
    d->id = i;
    d->duration = get_random_time(t1, t2, &i);
}

int get_totalmax_instances(int x, int y, int z) {
    if (x <= y/3 && x <= z) {
        return x;
    } 
    else if (y/3 <= x && y/3 <= z) {
        return y/3;
    }
    else {
        return z;
    }
}

void* dungeon_start(void* arg) {
    dungeon_instance* dungeon = (dungeon_instance*)arg;

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time); // Record the start time

    printf("Dungeon %d: ACTIVE\n", dungeon->id);

    #ifdef _WIN32
        Sleep(dungeon->duration * 1000);
    #else
        sleep(dungeon->duration * 100000);
    #endif

    pthread_mutex_lock(&data_q_lock);
    data_q.tank_q_head++;
    data_q.dps_q_head+=3;
    data_q.heal_q_head++;
    pthread_mutex_unlock(&data_q_lock);

    clock_gettime(CLOCK_MONOTONIC, &end_time); // Record the end time

    // Calculate the duration in seconds
    double duration = (end_time.tv_sec - start_time.tv_sec) +
                      (end_time.tv_nsec - start_time.tv_nsec) / 1e9;

    printf("Dungeon %d: EMPTY || Finished in %.9f seconds\n", dungeon->id, duration);

    total_duration[dungeon->id] += duration;

    sem_post(&dungeon_semaphore);

    return NULL;
}

int get_random_time(int t1, int t2, unsigned int* seed) {
    srand(*seed);
    return t1 + rand() % (t2 - t1 + 1);
}

int* get_remaining_players(int min) {
    int* remaining_players = malloc(3 * sizeof(int));
    remaining_players[0] = data_q.tank_q_tail - min;
    remaining_players[1] = (data_q.dps_q_tail - (min*3));
    remaining_players[2] = data_q.heal_q_tail - min;
    // printf("%d %d %d\n", data_q.tank_q_tail, data_q.dps_q_tail, data_q.heal_q_tail);
    // printf("%d %d %d\n", remaining_players[0], remaining_players[1], remaining_players[2]);

    return remaining_players;
}

void display_loading(char* header, char* end_message) {
    #ifdef _WIN32
        Sleep(500); // Windows-specific (500 ms)
        // system("cls");
    #else
        usleep(500000); // POSIX-specific (500 ms)
        // system("clear");
    #endif
    printf("\n");
    for (int i = 0; i < 1000; i++) {
        if (i == 0) {
            printf("\r%s\t[-------------------------------------------------]  0%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500); // Windows-specific (500 ms)
                // system("cls");
            #else
                usleep(500000); // POSIX-specific (500 ms)
                // system("clear");
            #endif
        }
        if (i == 123) {
            printf("\r%s\t[=====--------------------------------------------]  12%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500);
                // system("cls");
            #else
                usleep(500000);
                // system("clear");
            #endif
        }
        if (i == 456) {
            printf("\r%s\t[=====================----------------------------]  45%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500);
                // system("cls");
            #else
                usleep(500000);
                // system("clear");
            #endif
        }
        if (i == 619) {
            printf("\r%s\t[================================-----------------]  61%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500);
                // system("cls");
            #else
                usleep(500000);
                // system("clear");
            #endif
        }
        if (i == 789) {
            printf("\r%s\t[=====================================------------]  78%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500);
                // system("cls");
            #else
                usleep(500000);
                // system("clear");
            #endif
        }
        if (i == 927) {
            printf("\r%s\t[==============================================---]  92%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500);
                // system("cls");
            #else
                usleep(500000);
                // system("clear");
            #endif
        }
        if (i == 999) {
            printf("\r%s\t[================================================-] 99%%", header);
            // Delay and clear screen
            #ifdef _WIN32
                Sleep(500);
            #else
                usleep(500000);
            #endif
        }
    }

    printf("\r%s\t[=================================================] 100%%\n", header);
    printf("%s\n", end_message);
}

void display_results(input_attr input) {
    printf("\n\n");
    printf("[Final Stats]\n");
    for (int i = 0; i < input.n; i++) {
        printf("Dungeon %d: %d parties served || %.9f seconds\n", i, total_parties_served[i], total_duration[i]);
    }
    printf("\n");
    int* remaining_players = get_remaining_players(get_totalmax_instances(input.tank_n, input.dps_n, input.heal_n));
    printf("Number of unqueued players:\n");
    printf("Tank: %d\n", remaining_players[0]);
    printf("DPS: %d\n", remaining_players[1]);
    printf("Healer: %d\n", remaining_players[2]);
}