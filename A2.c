/*
Program: Sleeping TA Problem Simulation (filename: A2.c)
Description:
    Simulates students alternating between programming and seeking help from a TA.
    Uses pthreads, mutexes, and semaphores to handle synchronization with limited chairs.
    Runs indefinitely; please terminate manually with Ctrl+C.
*/

/*
Compile:
    gcc -pthread A2.c -o A2

Run:
    ./A2 <number_of_students>

Example:
    ./A2 5
*/

#include <stdio.h>             // printf()
#include <stdlib.h>            // malloc(), free(), atoi(), rand()
#include <pthread.h>           // pthread_create(), pthread_join(), 
                               // pthread_mutex_init(), pthread_mutex_destroy(),
                               // pthread_mutex_lock(), pthread_mutex_unlock(), 
                               // pthread_mutex_t, pthread_t
#include <semaphore.h>         // sem_init(), sem_wait(), sem_post(), sem_destroy()
                               // sem_t 
#include <unistd.h>            // sleep()
#include <time.h>              // time()

#define CHAIRS 3   // Number of hallway chairs

// ---------------- GLOBAL VARIABLES ----------------

// Mutex to protect shared variables (queue and waiting_students)
pthread_mutex_t mutex;

// Semaphore: counts waiting students; TA waits if zero
sem_t students_sem;

// Semaphore: student waits for TA to be ready
sem_t ta_sem;

// Shared counters
int waiting_students = 0;  // Number of students currently waiting
int total_students;        // Total number of students

// FIFO queue for student IDs sitting in the hallway
int queue[CHAIRS];
int front = 0;  // Index to dequeue next student
int rear = 0;   // Index to enqueue next student

// ---------------- UTILITY FUNCTIONS ----------------

// Add a student to the queue
void enqueue_student(int id) {
    queue[rear] = id;
    rear = (rear + 1) % CHAIRS;
}

// Remove a student from the queue
int dequeue_student() {
    int id = queue[front];
    front = (front + 1) % CHAIRS;
    return id;
}

// ---------------- TA THREAD FUNCTION ----------------
void* teaching_assistant(void* arg) {
    while (1) {
        sem_wait(&students_sem);       // Wait for a student to arrive

        pthread_mutex_lock(&mutex);
        int student_id = dequeue_student();  // Get next student
        waiting_students--;              // Only decrement when helping starts
        printf("TA is helping student %d. Waiting students = %d\n", student_id, waiting_students);
        pthread_mutex_unlock(&mutex);

        sem_post(&ta_sem);             // Signal TA is ready
        sleep(rand() % 3 + 1);         // Simulate helping (1–3 sec)
    }
    return NULL;
}

// ---------------- STUDENT THREAD FUNCTION ----------------
void* student(void* arg) {
    int id = *((int*)arg);
    free(arg);

    while (1) {
        printf("Student %d is programming.\n", id);
        sleep(rand() % 5 + 1);         // Simulate programming (1–5 sec)

        pthread_mutex_lock(&mutex);
        if (waiting_students < CHAIRS) {       // Check if chairs available
            enqueue_student(id);             // Sit in a chair
            waiting_students++;            // Increment while holding mutex
            printf("Student %d is waiting. Waiting students = %d\n", id, waiting_students);  
            sem_post(&students_sem);    // Notify TA
            pthread_mutex_unlock(&mutex);

            sem_wait(&ta_sem);         // Wait for TA to be ready
        }
        else {
            printf("Student %d found no empty chair and will return later.\n", id);
            pthread_mutex_unlock(&mutex); // No chair, leave mutex
        }
    }
    return NULL;
}

// ---------------- MAIN FUNCTION ----------------
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <number_of_students>\n", argv[0]);
        return 1;
    }

    total_students = atoi(argv[1]);
    pthread_t ta_thread;
    pthread_t student_threads[total_students];

    srand(time(NULL));

    // Initialize mutex and semaphores
    pthread_mutex_init(&mutex, NULL);
    sem_init(&students_sem, 0, 0);
    sem_init(&ta_sem, 0, 0);

    pthread_create(&ta_thread, NULL, teaching_assistant, NULL); // TA thread

    // Create student threads
    for (int i = 0; i < total_students; i++) {
        int* id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&student_threads[i], NULL, student, id);
    }

    pthread_join(ta_thread, NULL);  // Join TA thread (runs infinitely)
    for (int i = 0; i < total_students; i++) {
        pthread_join(student_threads[i], NULL);
    }

    // Cleanup (never reached)
    pthread_mutex_destroy(&mutex);
    sem_destroy(&students_sem);
    sem_destroy(&ta_sem);

    return 0;
}