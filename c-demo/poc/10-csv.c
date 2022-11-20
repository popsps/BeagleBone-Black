#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_column(char* line, int index, char* seperator) {
  char* token;
  char* res = NULL;
  int count = 0;
  token = strtok(line, seperator);
  while (token != NULL && count < index) {
    printf("token_: %s\n", token);
    token = strtok(NULL, seperator);
    count++;
  }
  if (token != NULL && count == index) {
    res = token;
  }
  return res;
}

int main(int argc, char* argv[]) {
  FILE* my_csv = fopen("my_csv.csv", "r");
  char line[1024];
  int count = 0;
  while (fgets(line, 1024, my_csv)) {
    count++;
    char* lineDup = strdup(line);
    printf("lineDup %p %s\n", lineDup, lineDup);
    printf("line %p %s\n", line, line);
    free(lineDup);
  }
  printf("Number of lines read: %d\n", count);
  printf("==========================================\n");
  printf("line %p %s\n", line, line);
  printf("Column: %s\n", get_column(line, 43, ","));

  printf("line %p %s\n", line, line);
  return 0;
}