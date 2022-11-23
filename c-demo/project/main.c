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
#include "uart.h"

#define TEN_MS 10000
#define HUNDRED_MS 100000
#define TWENTY_FOUR_HOURS_S 86400
#define TWENTY_FOUR_HOURS_MS 86400000

#define TEMP_SENSOR_PIN 1

#define ON_LIGHT P9_27
#define OFF_LIGHT P9_23
#define ON_OFF_BUTTON_PIN P9_15

void clean_up();
void initialize();
void unset_signal();
void sig_handler(int sig);

static int isOn = 1;
static int on_off_button_pressed = 0;
static int isResetButtonPressed = 0;
static double millivolts, temp_c, temp_f = 0;
static char altitude_str[256] = {0};
static char latitude_str[256] = {0};
static char latitude_dir[24] = {0};
static char longitude_str[256] = {0};
static char longitude_dir[24] = {0};
static char altitude_str[256] = {0};
static char number_of_satellites_str[256] = {0};
static int fix = 0;
static pthread_t main_thread, action_thread, temperature_thread, gps_thread, logger_thread;

void* handle_temperature_sensor(void* ptr);
void* handle_gps_sensor(void* ptr);
void* handle_actions(void* ptr);
void* handle_logger(void* ptr);

void toggle_lights();
void toggle_running();
void init_thread();
void init_work_space();
void clean_work_space();
void init_threads();
void destroy_threads();
char* get_nmea_field(char* nmea, int index);

pthread_rwlock_t isOn_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t temp_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t gps_rwlock = PTHREAD_RWLOCK_INITIALIZER;

int main(int argc, char* argv[]) {
  main_thread = pthread_self();
  init_work_space();
  b_log(DEBUG, "[THREAD%ld-MAIN]: Starting High Altitude Balloon Data Logger", main_thread);
  init_threads();
  destroy_threads();
  return 0;
}
void init_work_space() {
  struct stat std = {0};
  struct stat stl = {0};
  // create the data directory
  if (stat("./data", &std)) {
    b_log(DEBUG, "[THREAD%ld-MAIN]: Instantiating the Data Directory", main_thread);
    mkdir("./data", 0700);
  }
  // create the log directory
  if (stat("./log", &stl)) {
    b_log(DEBUG, "[THREAD%ld-MAIN]: Instantiating the Log Directory", main_thread);
    mkdir("./log", 0700);
  }
  csv_init();
  logger_init();

  // init signal handler for SIGUSR1, SIGINT, and SIGTSTP
  register_signal_handler(sig_handler);
  clean_up();
  initialize();
}
void clean_work_space() {
  logger_close();
  cvs_close();
}
void init_threads() {
  pthread_create(&temperature_thread, NULL, handle_temperature_sensor, NULL);
  pthread_create(&action_thread, NULL, handle_actions, NULL);
  pthread_create(&gps_thread, NULL, handle_gps_sensor, NULL);
  pthread_create(&logger_thread, NULL, handle_logger, NULL);
}
void destroy_threads() {
  pthread_join(temperature_thread, NULL);
  pthread_join(gps_thread, NULL);
  pthread_join(logger_thread, NULL);
  pthread_join(action_thread, NULL);
}
void clean_up() {
  gpio_unexport(ON_LIGHT);
  gpio_unexport(OFF_LIGHT);
  gpio_unexport(ON_OFF_BUTTON_PIN);
}
void unset_pins() {
  gpio_set_value(ON_LIGHT, LOW);
  gpio_set_value(OFF_LIGHT, LOW);
  gpio_set_value(ON_OFF_BUTTON_PIN, LOW);
}
void initialize() {
  gpio_export(ON_LIGHT);
  gpio_set_direction(ON_LIGHT, OUTPUT_PIN);
  gpio_set_value(ON_LIGHT, HIGH);

  gpio_export(OFF_LIGHT);
  gpio_set_direction(OFF_LIGHT, OUTPUT_PIN);
  gpio_set_value(OFF_LIGHT, LOW);

  gpio_export(ON_OFF_BUTTON_PIN);
  gpio_set_direction(ON_OFF_BUTTON_PIN, INPUT_PIN);
  gpio_set_value(ON_OFF_BUTTON_PIN, LOW);
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
    clean_work_space();
    shell_write(BPURPLE, "[THREAD%ld]: Pins are cleaned up.", current_thread);
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGTSTP) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGTSTP", current_thread);
    unset_pins();
    unset_pins();
    clean_work_space();
    shell_write(BPURPLE, "[THREAD%ld]: Pins are cleaned up.", current_thread);
    _exit(EXIT_SUCCESS);
  } else if (sig == SIGUSR1) {
    shell_write(BPURPLE, "[THREAD%ld]: Recived SIGUSR1", current_thread);
  }
}

/**
 * handle logic for the temperature sensor
 */
void* handle_temperature_sensor(void* ptr) {
  b_log(DEBUG, "[THREAD%ld-ACTION]: Starting the TEMPERATURE THREAD...", temperature_thread);
  while (1) {
    pthread_rwlock_rdlock(&isOn_rwlock);
    if (isOn) {
      pthread_rwlock_wrlock(&temp_rwlock);
      // read analog input AIN{1}
      int analog_input_value = read_analog(TEMP_SENSOR_PIN);
      // convert analog input to temperature value
      millivolts = analog_input_value / 4096.0f * 1800;
      temp_c = (millivolts - 500) / 10.0;
      temp_f = (temp_c * 9 / 5) + 32;
      pthread_rwlock_unlock(&temp_rwlock);
    }
    pthread_rwlock_unlock(&isOn_rwlock);
    sleep(1);
  }
  return NULL;
}
/**
 * handle logic for the gps sensor
 */
void* handle_gps_sensor(void* ptr) {
  b_log(DEBUG, "[THREAD%ld-ACTION]: Starting the GPS THREAD...", gps_thread);
  uart_init(4);

  while (1) {
    char* nmea = serial_read_line();
    if (nmea != NULL && nmea[0] != '\0' && nmea[0] != '\n') {
      // b_log(INFO, "[THREAD%ld-NMEA]: %s", gps_thread, nmea);
      // b_log(DEBUG, "[THREAD%ld-NMEA]: %s", gps_thread, nmea);
      // if NMEA is GPRMC
      if (strstr(nmea, "$GPRMC") != NULL) {
        char* lat = get_nmea_field(nmea, 3);
        char* lon = get_nmea_field(nmea, 5);
        char* lat_d = get_nmea_field(nmea, 4);
        char* lon_d = get_nmea_field(nmea, 6);
        if (lat != NULL && lon != NULL) {
          // b_log(INFO, "[THREAD%ld-NMEA]: %s, %s", gps_thread, lat, lon);
          pthread_rwlock_wrlock(&gps_rwlock);
          strcpy(latitude_str, lat);
          strcpy(longitude_str, lon);
          strcpy(latitude_dir, lat_d);
          strcpy(longitude_dir, lon_d);
          pthread_rwlock_unlock(&gps_rwlock);
        }
      } else if (strstr(nmea, "$GPGGA") != NULL) {
        char* fix_str = get_nmea_field(nmea, 6);
        char* _number_of_satellites_str = get_nmea_field(nmea, 7);
        char* _altitude_str = get_nmea_field(nmea, 9);
        pthread_rwlock_wrlock(&gps_rwlock);
        b_log(DEBUG, "[THREAD%ld-NMEA]: fix nmea %s", gps_thread, nmea);
        // b_log(DEBUG, "[THREAD%ld-NMEA]: fix: %s %s %s", gps_thread, fix_str, _number_of_satellites_str, _altitude_str);
        strcpy(number_of_satellites_str, _number_of_satellites_str);
        strcpy(altitude_str, _altitude_str);
        fix = atoi(fix_str);
        // b_log(INFO, "[THREAD%ld-NMEA]: fix: %d %s %s", gps_thread, fix, number_of_satellites_str, altitude_str);
        pthread_rwlock_unlock(&gps_rwlock);
      }
      free(nmea);
    }
  }
  return NULL;
}

/**
 * handle logic for the logging and writing to cvs file
 */
void* handle_logger(void* ptr) {
  b_log(DEBUG, "[THREAD%ld-ACTION]: Starting the LOGGER THREAD...", logger_thread);
  while (1) {
    pthread_rwlock_rdlock(&isOn_rwlock);
    // debug
    if (isOn) {
      pthread_rwlock_rdlock(&temp_rwlock);
      // b_log(DEBUG, "[THREAD%ld-LOGGER]: mv=%.2f C=%.2f F=%.2f", logger_thread, millivolts, temp_c, temp_f);
      pthread_rwlock_unlock(&temp_rwlock);
      pthread_rwlock_rdlock(&gps_rwlock);
      // if GPS is working and its values are valid
      if (fix != 0) {
        b_log(INFO, "[THREAD%ld-NMEA]: [latitude, longitude, temp]: %s %s, %s %s, %s", logger_thread, latitude_str,
              latitude_dir, longitude_str, longitude_dir, temp_c);
        //  atof(lat), atof(lon)
        log_csv(4, latitude_str, latitude_dir, longitude_str, longitude_dir, temp_c);
      }
      pthread_rwlock_unlock(&gps_rwlock);
    }
    pthread_rwlock_unlock(&isOn_rwlock);
    sleep(1);
  }
  return NULL;
}

/**
 * handle logic for user actions
 */
void* handle_actions(void* ptr) {
  b_log(DEBUG, "[THREAD%ld-ACTION]: Starting the ACTION THREAD...", action_thread);
  while (1) {
    int _on_off_button_pressed = gpio_get_value(ON_OFF_BUTTON_PIN);
    if (_on_off_button_pressed && !on_off_button_pressed) {
      on_off_button_pressed = 1;
      pthread_rwlock_wrlock(&isOn_rwlock);
      toggle_running();
      toggle_lights();
      if (isOn) {
        b_log(DEBUG, "[THREAD%ld-ACTION]: application is started.", action_thread);
      } else {
        b_log(DEBUG, "[THREAD%ld-ACTION]: application is stopped.", action_thread);
      }
      pthread_rwlock_unlock(&isOn_rwlock);
    }
    if (!_on_off_button_pressed) {
      on_off_button_pressed = 0;
    }
    // Read buttons every 10ms
    usleep(TEN_MS);
  }
  return NULL;
}
/**
 * toggling the lights
 * green indicates that the application is on and logs gps and temperature data
 * red indicates that the application is off
 */
void toggle_lights() {
  if (isOn) {
    gpio_set_value(OFF_LIGHT, LOW);
    gpio_set_value(ON_LIGHT, HIGH);
  } else {
    gpio_set_value(OFF_LIGHT, HIGH);
    gpio_set_value(ON_LIGHT, LOW);
  }
}
/**
 * toggle on/off the application
 */
void toggle_running() { isOn = !isOn; }

char* get_nmea_field(char* nmea, int index) {
  char* nmea_dup = strdup(nmea);
  char* seperator = ",";
  char* token;
  char* res = NULL;
  int count = 0;
  token = strtok(nmea_dup, seperator);
  while (token != NULL && count < index) {
    token = strtok(NULL, seperator);
    count++;
  }
  if (token != NULL && count == index) {
    res = token;
  }
  free(nmea_dup);
  return res;
}