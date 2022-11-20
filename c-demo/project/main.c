#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "bbsignal.h"
#include "gpio.h"
#include "logging.h"
#include "pins.h"

#define TEN_MS 10000
#define HUNDRED_MS 100000
#define TWENTY_FOUR_HOURS_S 86400
#define TWENTY_FOUR_HOURS_MS 86400000

#define START_LIGHT_PIN P9_12
#define TEMP_SENSOR_PIN 1
#define STOP_LIGHT_PIN P9_11
#define START_STOP_BUTTON_PIN P9_15
#define RESET_BUTTON_PIN P9_27

void clean_up();
void initialize();
void unset_signal();
void sig_handler(int sig);

static uint64_t counter = 0;
static int isRunning = 0;
static int isStartStopButtonPressed = 0;
static int isResetButtonPressed = 0;
static double millivolts, temp_c, temp_f = 0;
static pthread_t main_thread, temperature_thread, gps_thread, logger_thread;

void* handle_temperature_sensor(void* ptr);
void* handle_gps_sensor(void* ptr);
void* logger_handler(void* ptr);

void toggle_running();
void toggle_lights();
void reset_timer();
void init_thread();
void init_work_space();
void init_threads();
void destroy_threads();

pthread_rwlock_t isRunning_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t counter_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int main(int argc, char* argv[]) {
  main_thread = pthread_self();
  shell_print(BBLACK, "[THREAD%ld-MAIN]: Final Project", main_thread);
  init_work_space();
  init_threads();
  FILE* fp = fopen("./data/scores.dat", "a+");
  fprintf(fp, "2,afew,afew,234,234m234\n");
  fprintf(fp, "4,hrtht,htre,543,bs345\n");
  destroy_threads();
  return 0;
}
void init_work_space() {
  struct stat st = {0};
  if (stat("./data", &st)) {
    shell_print(BBLACK, "[THREAD%ld-MAIN]: Instantiating the Data Directory", main_thread);
    mkdir("./data", 0700);
  }
}
void init_threads() {
  pthread_create(&temperature_thread, NULL, handle_temperature_sensor, NULL);
  pthread_create(&gps_thread, NULL, handle_gps_sensor, NULL);
  pthread_create(&logger_thread, NULL, logger_handler, NULL);
}
void destroy_threads() {
  pthread_join(temperature_thread, NULL);
  pthread_join(gps_thread, NULL);
  pthread_join(logger_thread, NULL);
}
void clean_up() {
  gpio_unexport(START_LIGHT_PIN);
  gpio_unexport(STOP_LIGHT_PIN);
  gpio_unexport(START_STOP_BUTTON_PIN);
  gpio_unexport(RESET_BUTTON_PIN);
}
void unset_pins() {
  gpio_set_value(START_LIGHT_PIN, LOW);
  gpio_set_value(STOP_LIGHT_PIN, LOW);
  gpio_set_value(START_STOP_BUTTON_PIN, LOW);
  gpio_set_value(RESET_BUTTON_PIN, LOW);
}
void initialize() {
  gpio_export(STOP_LIGHT_PIN);
  gpio_set_direction(STOP_LIGHT_PIN, OUTPUT_PIN);
  gpio_set_value(STOP_LIGHT_PIN, HIGH);

  gpio_export(START_LIGHT_PIN);
  gpio_set_direction(START_LIGHT_PIN, OUTPUT_PIN);
  gpio_set_value(START_LIGHT_PIN, LOW);

  gpio_export(START_STOP_BUTTON_PIN);
  gpio_set_direction(START_STOP_BUTTON_PIN, INPUT_PIN);
  gpio_set_value(START_STOP_BUTTON_PIN, LOW);

  gpio_export(RESET_BUTTON_PIN);
  gpio_set_direction(RESET_BUTTON_PIN, INPUT_PIN);
  gpio_set_value(RESET_BUTTON_PIN, LOW);
}

/**
 * handle OS signals
 * handle SIGINT CTRL+C
 * handle SIGTSTP CTRL+Z
 * handle SIGUSR1 custom signal to bypass sleep on a thread
 **/
void sig_handler(int sig) {
  pthread_t current_thread = pthread_self();
  if (sig == SIGINT) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGINT", current_thread);
    unset_pins();
    unset_pins();
    shell_write(BPURPLE, "[THREAD%ld]: Pins are cleaned up.", current_thread);
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGTSTP) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGTSTP", current_thread);
    unset_pins();
    unset_pins();
    shell_write(BPURPLE, "[THREAD%ld]: Pins are cleaned up.", current_thread);
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGUSR1) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGUSR1", current_thread);
  }
}

void* handle_temperature_sensor(void* ptr) {
  // read analog input AIN{1}
  int analog_input_value = read_analog(TEMP_SENSOR_PIN);
  // convert analog input to temperature value
  millivolts = analog_input_value / 4096.0f * 1800;
  temp_c = (millivolts - 500) / 10.0;
  temp_f = (temp_c * 9 / 5) + 32;
}
void* handle_gps_sensor(void* ptr) { return NULL; }
void* logger_handler(void* ptr) {
  shell_print(KDEF, "[THREAD%ld-TEMPERATURE]: mv=%.2f C=%.2f F=%.2f", logger_thread, millivolts, temp_c, temp_f);
}