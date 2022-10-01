#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT_DONE 10
#define COUNT_HALT1 3
#define COUNT_HALT2 6

int counter = 0;
int count = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;

void *print_message(void *ptr);

int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;
  char *message1 = "Thread 1";
  char *message2 = "Thread 2";

  int iret1 = pthread_create(&thread1, NULL, print_message, (void *)message1);
  int iret2 = pthread_create(&thread2, NULL, print_message, (void *)message2);

  printf("[Main thread]: %d, %d.\n", iret1, iret2);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("Counter: %d\n", counter);
  printf("[Main thread]: All done for %d, %d.\n", iret1, iret2);
  exit(0);
}

void *print_message(void *ptr) {
  char *message = (char *)ptr;
  printf("Message: %s\n", message);
  pthread_mutex_lock(&mutex1);
  counter++;
  pthread_mutex_unlock(&mutex1);
}

void func1() {
  while (1) {
    pthread_mutex_lock(&condition_mutex);
    while (count >= COUNT_HALT1 && count <= COUNT_HALT2) {
      pthread_cond_wait(&condition_cond, &condition_mutex);
    }
    pthread_mutex_unlock(&condition_mutex);
  }
}