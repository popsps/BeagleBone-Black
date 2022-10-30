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

/**
 * Each side gets green light for 2 minutes. The transition to the red light
 * should have a 5 second yellow signal.
 *
 * Using GPIO 9_11 and GPIO 9_12 GPIO 9_13 as out pins for signal 1
 * Using GPIO 9_23 and GPIO 9_24 GPIO 9_26 as out pins for signal 2
 * Using GPIO 9_15 as in pin for signal 1
 * Using GPIO 9_41 as int pin for signal 2
 *
 * One thread for each of the traffic signals
 * Add a wait sensor sensor at each direction
 * there should be one thread for these as well Implement this with a
 * push-button If a light is red but the corresponding wait sensor is triggered
 * (i.e. the button is pushed and held) for 5 seconds,
 * the light should cycle to green without waiting the full 2 minutes
 * 10ms usleep(10000)
 * 100ms usleep(100000)
 * time_t type as a signed integer
 **/

#define RG_WAIT_TIME 120
#define Y_WAIT_TIME 5
#define SMALL_WAIT_TIME 1
#define SENSOR_ACTIVATION_TIME 5

#define START_LIGHT_PIN P9_12
#define STOP_LIGHT_PIN P9_11
#define START_STOP_BUTTON_PIN P9_15
#define RESET_BUTTON_PIN P9_27

pthread_rwlock_t timer_rwlock = PTHREAD_RWLOCK_INITIALIZER;

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

pthread_rwlock_t signal_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int main(int argc, char* argv[]) {
  pthread_t current_thread = pthread_self();
  shell_print(BBLACK, "[THREAD%ld-MAIN]: lab4 Simple Intersection with Opposing Traffic Lights", current_thread);

  // init signal handler for SIGUSR1, SIGINT, and SIGTSTP
  register_signal_handler(sig_handler);
  // init signal blocker
  register_sigpromask();

  // clean up and unset any previous state of GPIOs and re-initialize them again
  clean_up();
  initialize();

  // threads responsible for handling the general intersection logic
  pthread_create(&timer_thread, NULL, handle_timer, NULL);
  pthread_create(&terminal_thread, NULL, handle_action, NULL);
  pthread_create(&action_thread, NULL, handle_action, NULL);

  pthread_join(timer_thread, NULL);
  pthread_join(terminal_thread, NULL);
  pthread_join(action_thread, NULL);

  shell_print(BBLACK,
              "[THREAD%ld-MAIN]: lab4 Simple Intersection with Opposing "
              "Traffic Lights Done",
              current_thread);
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

void* handle_timer(void* ptr) {
  shell_print(BBLUE, "[THREAD%ld-TIMER]: Starting the TIMER thread...", timer_thread);
  sleep(1);
  // time_t base = time(0);
  // time_t now = base;
  // struct timespec base, now;
  // clock_gettime(CLOCK_MONOTONIC_RAW, &base);
  // do stuff
  while (1) {
    sleep(1);
    // usleep(10000);
    // now = time(0);
    // printf("now: %ld, base: %ld, now - base: %ld\n", now, base, now - base);
    // clock_gettime(CLOCK_MONOTONIC_RAW, &now);
    // uint64_t delta_us = (now.tv_sec - base.tv_sec) * 1000000 + (now.tv_nsec - base.tv_nsec) / 1000;
    // printf("base [%ld, %ld]: \n", base.tv_sec, base.tv_nsec);
    // printf("now [%ld, %ld]:\n", now.tv_sec, now.tv_nsec);
    // printf("delta_us: %lld\n", delta_us);
    shell_print(BBLUE, "[THREAD%ld-TIMER]: %lld", timer_thread, counter);
    if(isRunning) {
      counter++;
    }
  }
}
void* handle_action(void* ptr) {
  shell_print(BRED, "[THREAD%ld-ACTION]: starting ACTION thread...", action_thread);
  while (1) {
    int _isStartStopButtonPressed = gpio_get_value(START_STOP_BUTTON_PIN);
    if (_isStartStopButtonPressed && !isStartStopButtonPressed) {
      isStartStopButtonPressed = 1;
      shell_print(BRED, "[THREAD%ld-ACTION]: start/stop button is pressed...", action_thread);
      toggle_running();
      toggle_lights();
    } 
    if(!_isStartStopButtonPressed) {
      isStartStopButtonPressed = 0;
    }
    int _isResetButtonPressed = gpio_get_value(RESET_BUTTON_PIN);
    if (_isResetButtonPressed&& !isResetButtonPressed) {
      isResetButtonPressed = 1;
      reset_timer();
    }
    if(!_isResetButtonPressed) {
      isResetButtonPressed = 0;
    }
    // Read buttons every 10ms
    usleep(10000);
  }
}
void* handle_terminal(void* ptr) {
  shell_print(BBLUE, "[THREAD%ld-TERMINAL]: Starting the TIMER thread...", terminal_thread);
  sleep(1);
  // uint64_t u = 4294967293;
  while (1) {
    // shell_print(BBLUE, "[THREAD%ld-TIMMER]: %zu", timer_thread, u);
    shell_print(BBLUE, "\r[THREAD%ld-TERMINAL]: %u", terminal_thread, counter);
    // printf("\rValue of COUNTER: %lu", counter);
    // fflush(stdout);
    usleep(10000);
    // sleep(1);
    counter++;
  }
}

void toggle_running() {
  isRunning = !isRunning;
  if(isRunning) {
    shell_print(BRED, "[THREAD%ld-ACTION]: timer is started.", action_thread);
  } else {
    shell_print(BRED, "[THREAD%ld-ACTION]: timer is stopped.", action_thread);
  }
 }
void toggle_lights() {
  if (isRunning) {
    gpio_set_value(STOP_LIGHT_PIN, LOW);
    gpio_set_value(START_LIGHT_PIN, HIGH);
  } else {
    gpio_set_value(STOP_LIGHT_PIN, HIGH);
    gpio_set_value(START_LIGHT_PIN, LOW);
  }
}
void reset_timer() { counter = 0; }