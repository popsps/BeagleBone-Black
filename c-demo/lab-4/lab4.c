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
 * 10ms usleep(10000)
 * 100ms usleep(100000)
 * time_t type as a signed integer
 **/

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

  pthread_attr_t timer_attr, terminal_attr, action_attr;
  struct sched_param timer_param, terminal_param, action_param;

  pthread_attr_init(&timer_attr);
  pthread_attr_setschedpolicy(&timer_attr, SCHED_FIFO);
  // pthread_attr_getschedparam(&timer_attr, &timer_param);
  timer_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  // pthread_attr_getschedpolicy(&timer_attr, &policy);
  pthread_attr_setschedparam(&timer_attr, &timer_param);

  pthread_attr_init(&terminal_attr);
  pthread_attr_setschedpolicy(&terminal_attr, SCHED_FIFO);
  terminal_param.sched_priority = 20;
  pthread_attr_setschedparam(&terminal_attr, &terminal_param);

  pthread_attr_init(&action_attr);
  pthread_attr_setschedpolicy(&action_attr, SCHED_FIFO);
  action_param.sched_priority = 45;
  pthread_attr_setschedparam(&action_attr, &action_param);

  shell_print(BBLACK, "[THREAD%ld-MAIN]: TIMER THREAD PRIORITY: %d", current_thread, timer_param.sched_priority);
  shell_print(BBLACK, "[THREAD%ld-MAIN]: ACTION THREAD PRIORITY: %d", current_thread, action_param.sched_priority);
  shell_print(BBLACK, "[THREAD%ld-MAIN]: TERMINAL THREAD PRIORITY: %d", current_thread, terminal_param.sched_priority);

  // threads responsible for handling the stopwatch logic
  pthread_create(&timer_thread, &timer_attr, handle_timer, NULL);
  pthread_create(&terminal_thread, &terminal_attr, handle_terminal, NULL);
  pthread_create(&action_thread, &action_attr, handle_action, NULL);

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

/**
 * Handles the counter. The counter will be used in the terminal thread to display counter in the stopwatch
 */
void* handle_timer(void* ptr) {
  shell_print(KDEF, "[THREAD%ld-TIMER]: Starting the TIMER thread...", timer_thread);
  while (1) {
    // compute time in Mili-seconds
    pthread_rwlock_rdlock(&isRunning_rwlock);
    pthread_rwlock_wrlock(&counter_rwlock);
    if (isRunning) {
      counter = (counter >= TWENTY_FOUR_HOURS_MS) ? 0 : counter + 10;
    } else {
      counter = (counter >= TWENTY_FOUR_HOURS_MS) ? 0 : counter;
    }
    pthread_rwlock_unlock(&counter_rwlock);
    pthread_rwlock_unlock(&isRunning_rwlock);
    usleep(TEN_MS);
  }
}
/**
 * handle read buttons. this includes start/stop and reset buttons.
 * Set lights accordingly
 */
void* handle_action(void* ptr) {
  shell_print(BRED, "[THREAD%ld-ACTION]: starting ACTION thread...", action_thread);
  while (1) {
    int _isStartStopButtonPressed = gpio_get_value(START_STOP_BUTTON_PIN);
    if (_isStartStopButtonPressed && !isStartStopButtonPressed) {
      isStartStopButtonPressed = 1;
      shell_print(BRED, "[THREAD%ld-ACTION]: start/stop button is pressed...", action_thread);
      pthread_rwlock_wrlock(&isRunning_rwlock);
      toggle_running();
      if (isRunning) {
        shell_print(BRED, "[THREAD%ld-ACTION]: timer is started.", action_thread);
      } else {
        shell_print(BRED, "[THREAD%ld-ACTION]: timer is stopped.", action_thread);
      }
      toggle_lights();
      pthread_rwlock_unlock(&isRunning_rwlock);
    }
    if (!_isStartStopButtonPressed) {
      isStartStopButtonPressed = 0;
    }
    int _isResetButtonPressed = gpio_get_value(RESET_BUTTON_PIN);
    if (_isResetButtonPressed && !isResetButtonPressed) {
      isResetButtonPressed = 1;
      shell_print(BRED, "[THREAD%ld-ACTION]: reset button is pressed...", action_thread);
      pthread_rwlock_wrlock(&counter_rwlock);
      reset_timer();
      shell_print(BRED, "[THREAD%ld-ACTION]: timer is reset", action_thread);
      pthread_rwlock_unlock(&counter_rwlock);
    }
    if (!_isResetButtonPressed) {
      isResetButtonPressed = 0;
    }
    // Read buttons every 10ms
    usleep(TEN_MS);
  }
}

/**
 * Handle printing to the terminal. Updates every 100 MS.
 */
void* handle_terminal(void* ptr) {
  shell_print(BBLUE, "[THREAD%ld-TERMINAL]: Starting the TIMER thread...", terminal_thread);
  while (1) {
    pthread_rwlock_rdlock(&isRunning_rwlock);
    pthread_rwlock_rdlock(&counter_rwlock);
    double timer_value = counter / 1000.0;
    if (isRunning) {
      shell_print(BBLUE, "[THREAD%ld-TERMINAL]: TIMER: %.3f seconds", timer_thread, timer_value);
    } else {
      shell_print(BBLUE, "[THREAD%ld-TERMINAL]: TIMER: %.2f seconds", timer_thread, timer_value);
    }
    pthread_rwlock_unlock(&counter_rwlock);
    pthread_rwlock_unlock(&isRunning_rwlock);
    usleep(HUNDRED_MS);
  }
}

void toggle_running() { isRunning = !isRunning; }
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