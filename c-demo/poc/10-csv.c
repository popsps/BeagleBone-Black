#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

int sum(int n, ...) {
  va_list arg;
  int sum = 0;
  va_start(arg, n);

  for (size_t i = 0; i < n; i++) {
    sum += va_arg(arg, int);
  }

  va_end(arg);
  return sum;
}
void write_csv(FILE* fp, const int n, ...) {
  char buffer[1024] = {0};
  va_list arg;
  va_start(arg, n);
  for (size_t i = 0; i < n; i++) {
    char* field = va_arg(arg, char*);
    strncat(buffer, field, strlen(field));
    if (i < n - 1) {
      strcat(buffer, ",");
    }
  }
  va_end(arg);
  printf("BUFFER: %s\n", buffer);
  fprintf(fp, "%s\n", buffer);
  va_end(arg);
}

/**
 * fopen
 * r open for reading
 * w create empty for writing. if exists create new
 * a append. if exists create new
 * r+ reading and writing. file must exists
 * w+ reading and writing. if exists create new
 * a+ reading and appending
 */
int main(int argc, char* argv[]) {
  printf("sum: %d\n", sum(3, 3, 2, 6));
  FILE* my_csv = fopen("my_csv.csv", "a+");
  write_csv(my_csv, 4, "uuu", "kamapo livo", "342.43", "25");
  char line[1024];
  int count = 0;
  fseek(my_csv, 0, SEEK_SET);
  write_csv(my_csv, 4, "uuu", "kamapo livo", "342.43", "25");
  fseek(my_csv, 0, SEEK_SET);
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
  printf("Column 2: %s\n", get_column(line, 2, ","));
  printf("Column 43: %s\n", get_column(line, 43, ","));

  printf("line %p %s\n", line, line);
  fclose(my_csv);
  return 0;
}