#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int amount = 0;                  // shared variable
pthread_mutex_t lock;            // mutex

sem_t can_deposit;               // controls max balance
sem_t can_withdraw;              // controls min balance

void* deposit(void* param) {

    int deposit_amount = *((int*)param);

    printf("Executing deposit function\n");

    // Decreases semaphore value by 1
    sem_wait(&can_deposit);          // block if amount >= 400
    pthread_mutex_lock(&lock);

    amount += deposit_amount;
    printf("Amount after deposit = %d\n", amount);

    pthread_mutex_unlock(&lock);

    sem_post(&can_withdraw); 

    pthread_exit(NULL);
}

void* withdraw(void* param) {

    int withdraw_amount = *((int*)param);

    printf("Executing withdraw function\n");

    sem_wait(&can_withdraw);        // block if amount <= 0
    pthread_mutex_lock(&lock);

    amount -= withdraw_amount;
    printf("Amount after withdrawal = %d\n", amount);

    pthread_mutex_unlock(&lock);
    sem_post(&can_deposit); 

    pthread_exit(NULL);
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

    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }

    // initially deposits allowed, withdraw blocked
    if (sem_init(&can_deposit, 0, 4) != 0) {   // max 4 deposits allowed (400/100)
        printf("Semaphore can_deposit initialization failed\n");
        return 1;
    }

    if (sem_init(&can_withdraw, 0, 0) != 0) {
        printf("Semaphore can_withdraw initialization failed\n");
        return 1;
    }

    // Create Deposit Threads 
    for (int i = 0; i < 7; i++) {
        if (pthread_create(&threads[i], NULL, deposit, &transaction_amount) != 0) {
            printf("Error creating deposit thread %d\n", i);
        }
    }

    // Create Withdraw Threads
    for (int i = 7; i < 10; i++) {
        if (pthread_create(&threads[i], NULL, withdraw, &transaction_amount) != 0) {
            printf("Error creating withdraw thread %d\n", i);
        }
    }

    // Wait for all threads 
    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cleanup 
    pthread_mutex_destroy(&lock);
    sem_destroy(&can_deposit);
    sem_destroy(&can_withdraw);

    printf("Final amount = %d\n", amount);

    return 0;
}
