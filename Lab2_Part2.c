#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int amount = 0;               // shared variable
pthread_mutex_t lock;         // mutex for critical section

void* deposit(void* param) {
    int deposit_amount = *((int*)param);

    printf("Executing deposit function\n");

    pthread_mutex_lock(&lock);

    if (amount + deposit_amount <= 400) {
        amount += deposit_amount;
        printf("Amount after deposit = %d\n", amount);
    }

    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}

void* withdraw(void* param) {
    int withdraw_amount = *((int*)param);

    printf("Executing Withdraw function\n");

    pthread_mutex_lock(&lock);

    if (amount - withdraw_amount >= 0) {
        amount -= withdraw_amount;
        printf("Amount after Withdrawal = %d\n", amount);
    }

    pthread_mutex_unlock(&lock);
    pthread_exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <amount>\n", argv[0]);
        return 1;
    }

    int transaction_amount = atoi(argv[1]);
    if (transaction_amount != 100) {
        printf("Amount must be 100\n");
        return 1;
    }

    pthread_t threads[10];

    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    // Create 7 deposit threads
    for (int i = 0; i < 7; i++) {
        if (pthread_create(&threads[i], NULL, deposit, &transaction_amount) != 0) {
            printf("Error creating deposit thread %d\n", i);
        }
    }

    // Create 3 withdraw threads
    for (int i = 7; i < 10; i++) {
        if (pthread_create(&threads[i], NULL, withdraw, &transaction_amount) != 0) {
            printf("Error creating withdraw thread %d\n", i);
        }
    }

    // Wait for all threads
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    printf("Final amount = %d\n", amount);
    return 0;
}