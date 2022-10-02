#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Condition Variables
#define COUNT_DONE 10
#define COUNT_HALT1 3
#define COUNT_HALT2 6
int count = 0;

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_cond = PTHREAD_COND_INITIALIZER;
pthread_t thread1, thread2;
void *func1();
void *func2();

void sig_handler(int sig) {
  if (sig == SIGUSR1) {
    printf("Recived SIGUSR1\n");
  }
}
int main(int argc, char *argv[]) {
  struct sigaction new;
  new.sa_handler = sig_handler;
  sigaction(SIGUSR1, &new, NULL);

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
  time_t now = time(0);
  time_t base = now;
  while (1) {
    now = time(0);
    printf("Running [func 1] %ld...\n", now - base);
    if (now - base >= 2) {
      pthread_mutex_lock(&condition_mutex);
      pthread_cond_signal(&condition_cond);
      pthread_mutex_unlock(&condition_mutex);
      base = now;
      printf("Done [func 1] %ld...\n", now - base);
      // raise(SIGUSR1);
      pthread_kill(thread2, SIGUSR1);
    }

    usleep(400000);
  }
}

void *func2() {
  while (1) {
    printf("func 2 start ...\n");
    pthread_mutex_lock(&condition_mutex);
    pthread_cond_wait(&condition_cond, &condition_mutex);
    pthread_mutex_unlock(&condition_mutex);
    sleep(50);
    printf("func 2 recieved a signal\n");
  }
}