#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int amount = 0;                // shared variable
pthread_mutex_t lock;          // mutex for critical section

// Deposit function
void* deposit(void* param) {
    int deposit_amount = *((int*)param);

    pthread_mutex_lock(&lock);  // enter critical section
    amount += deposit_amount;
    printf("Deposit amount = %d\n", amount);
    pthread_mutex_unlock(&lock); // leave critical section

    pthread_exit(0);
}

// Withdraw function
void* withdraw(void* param) {
    int withdraw_amount = *((int*)param);

    pthread_mutex_lock(&lock);  // enter critical section
    amount -= withdraw_amount;
    printf("Withdrawal amount = %d\n", amount);
    pthread_mutex_unlock(&lock); // leave critical section

    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <deposit_amount> <withdraw_amount>\n", argv[0]);
        return 1;
    }

    int deposit_amt = atoi(argv[1]);
    int withdraw_amt = atoi(argv[2]);

    pthread_t threads[6];

    // Initialize the mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    // Create 3 deposit threads
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, deposit, &deposit_amt) != 0) {
            printf("Error creating deposit thread %d\n", i);
        }
    }

    // Create 3 withdraw threads
    for (int i = 3; i < 6; i++) {
        if (pthread_create(&threads[i], NULL, withdraw, &withdraw_amt) != 0) {
            printf("Error creating withdraw thread %d\n", i);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }

    // Destroy the mutex
    pthread_mutex_destroy(&lock);

    printf("Final amount = %d\n", amount);

    return 0;
}