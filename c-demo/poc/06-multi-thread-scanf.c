#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// mutex simple
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int counter = 0;

void *get_message(void *ptr);
void *print_message(void *ptr);
/**
 * gcc -lpthread threadtest.c -o threadtest
 * gcc -pthread threadtest.c -o threadtest
 **/
int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;
  char *message1 = "Thread 1";
  char *message2 = "Thread 2";

  int iret1 = pthread_create(&thread1, NULL, get_message, (void *)message1);
  int iret2 = pthread_create(&thread2, NULL, print_message, (void *)message2);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  printf("Counter: %d\n", counter);
  printf("[Main thread]: All done for %d, %d.\n", iret1, iret2);
  exit(0);
}

void *print_message(void *ptr) {
  while (1) {
    printf("print message function ....\n");
    sleep(1);
  }
}
void *get_message(void *ptr) {
  while (1) {
    int isHold = 0;
    char buffer[45] = {0};
    if (fgets(buffer, 45, stdin) != NULL) {
      isHold = atoi(buffer);
    }
    printf("get message function %d ....\n", isHold);
    sleep(1);
  }
}
