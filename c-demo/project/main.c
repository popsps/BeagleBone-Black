#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static pthread_t timer_thread, action_thread, terminal_thread;

void* handle_timer(void* ptr);
void* handle_terminal(void* ptr);
void* handle_action(void* ptr);

void toggle_running();
void toggle_lights();
void reset_timer();

pthread_rwlock_t isRunning_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t counter_rwlock = PTHREAD_RWLOCK_INITIALIZER;
double readTemperature() {
    int fd = op
}
int main(int argc, char* argv[]) {
  pthread_t current_thread = pthread_self();
  shell_print(BBLACK, "[THREAD%ld-MAIN]: lab4 Stopwatch", current_thread);

  return 0;
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
