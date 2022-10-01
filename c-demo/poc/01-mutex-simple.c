#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// mutex simple
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *print_message(void *ptr);
/**
 * gcc -lpthread threadtest.c -o threadtest
 * gcc -pthread threadtest.c -o threadtest
 **/
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