#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void* add(int a, int b) {
  int* res = malloc(sizeof(int));
  *res = a + b;
  return res;
};
void* subtract(int a, int b) {
  int* res = malloc(sizeof(int));
  *res = a - b;
  return res;
};
void* increment(void* ptr) {
  int a = *((int*)ptr);
  int* res = malloc(sizeof(int));
  *res = a + 1;
  return res;
}
void* print(void* ptr) {
  char* message = (char*)ptr;
  printf("[PRINT]: %s\n", message);
}

typedef void (*calc)(int, int);
typedef void* (*step)(void*);

int main(int argc, char* argv[]) {
  printf("steps...\n");
  step func[3] = {&print, &increment};
  char* message = "custom message";
  func[0]((void*)message);

  int* i = malloc(sizeof(int));
  *i = 123;
  //   func[1](i);
  int res = *((int*)func[1]((void*)i));
  printf("func: %d\n", res);

  exit(0);
}
