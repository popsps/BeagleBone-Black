#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_column(char* line, int index, char* seperator) {
  char* token;
  char* res = NULL;
  int count = 0;
  token = strtok(line, seperator);
  while (token != NULL && count < index) {
    token = strtok(NULL, seperator);
    count++;
  }
  if (token != NULL && count == index) {
    res = token;
  }
  return res;
}
