#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Condition Variables
#define COUNT_DONE 10
#define COUNT_HALT1 3
#define COUNT_HALT2 6
int count = 0;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;
void *func1();
void *func2();

int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;
  char *message1 = "Thread 1";
  char *message2 = "Thread 2";

  int iret1 = pthread_create(&thread1, NULL, func1, NULL);
  int iret2 = pthread_create(&thread2, NULL, func2, NULL);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("Count: %d\n", count);
  exit(0);
}

void *func1() {
  while (1) {
    // conditional waiting
    // if 3 <= count <= 6 wait
    pthread_mutex_lock(&condition_mutex);
    while (count >= COUNT_HALT1 && count <= COUNT_HALT2) {
      pthread_cond_wait(&condition_cond, &condition_mutex);
    }
    pthread_mutex_unlock(&condition_mutex);

    // write into the shared variable
    pthread_mutex_lock(&count_mutex);
    count++;
    printf("Counter value func1: %d\n", count);
    pthread_mutex_unlock(&count_mutex);
    if (count >= COUNT_DONE) return NULL;
  }
}

void *func2() {
  while (1) {
    // if count < 3 or count > 6 signal to other threads
    pthread_mutex_lock(&condition_mutex);
    if (count < COUNT_HALT1 || count > COUNT_HALT2) {
      pthread_cond_signal(&condition_cond);
    }
    pthread_mutex_unlock(&condition_mutex);

    // write into the shared variable
    pthread_mutex_lock(&count_mutex);
    count++;
    printf("Counter value func2: %d\n", count);
    pthread_mutex_unlock(&count_mutex);
    if (count >= COUNT_DONE) return NULL;
  }
}