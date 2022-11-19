#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termio.h>
#include <time.h>
#include <unistd.h>

#define LDR_PATH "/sys/bus/iio/devices/iio:device0/in_voltage"

int readAnalog(int pin) {
  int value = 0;
  char path[200];
  snprintf(path, sizeof(path), LDR_PATH "%d_raw", pin);
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    return -1;
  }
  int res = fscanf(file, "%d", &value);
  return value;
}

int readTemperature(int analogValue) {
  double millivolts = analogValue / 4096.0f * 1800;
  double temp_c = (millivolts - 500) / 10.0;
  double temp_f = (temp_c * 9 / 5) + 32;
  printf("mv=%.2f C=%.2f F=%.2f\n", millivolts, temp_c, temp_f);
}

int main(int argc, char* argv[]) {
  printf("reading analog input\n");
  int value = readAnalog(1);
  printf("Analog input value: %d\n", value);
  readTemperature(value);
  return 0;
}