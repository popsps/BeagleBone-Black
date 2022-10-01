#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Condition Variables
#define COUNT_DONE 10
#define COUNT_HALT1 3
#define COUNT_HALT2 6
int count = 0;

pthread_rwlock_t account_rwlock = PTHREAD_RWLOCK_INITIALIZER;
double checking_balance = 100.0;
double saving_balance = 100.0;

void *get_balance();

void *transfer_checking_to_saving(void *ptr);

int main(int argc, char *argv[]) {
  pthread_t thread1, thread2;
  char *message1 = "Thread 1";
  char *message2 = "Thread 2";

  int iret1 = pthread_create(&thread1, NULL, get_balance, NULL);
  double *amount = malloc(sizeof(*amount));
  *amount = 45.0;
  int iret2 = pthread_create(&thread2, NULL, transfer_checking_to_saving,
                             (void *)amount);

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  sleep(2);
  printf("checking: %f, saving: %f\n", checking_balance, saving_balance);
  exit(0);
}

void *get_balance() {
  pthread_rwlock_rdlock(&account_rwlock);
  double balance = checking_balance + saving_balance;
  printf("checking: %.2f, saving: %.2f, balance: %.2f\n", checking_balance,
         saving_balance, balance);
  pthread_rwlock_unlock(&account_rwlock);
}

void *transfer_checking_to_saving(void *ptr) {
  double amount = *((double *)ptr);
  printf("amount: %f\n", amount);
  pthread_rwlock_wrlock(&account_rwlock);
  checking_balance -= amount;
  saving_balance += amount;
  pthread_rwlock_unlock(&account_rwlock);
  free(ptr);
}