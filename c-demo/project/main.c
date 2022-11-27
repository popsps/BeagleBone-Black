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

void clean_up_gpio_pins();
void initialize_gpio_pins();
void unset_signal();
void sig_handler(int sig);

static int isOn = 1;
static int on_off_button_pressed = 0;
static int isResetButtonPressed = 0;
static double millivolts, temp_c, temp_f = 0;
static char GPRMC_NMEA[256] = {0};
static char GPGGA_NMEA[256] = {0};
static char altitude_str[256] = {0};
static char altitude_unit[256] = {0};
static char latitude_str[256] = {0};
static char latitude_hem[24] = {0};
static char longitude_str[256] = {0};
static char longitude_hem[24] = {0};
static char number_of_satellites_str[256] = {0};
static int fix = 0;
static pthread_t main_thread, action_thread, temperature_thread, gps_thread, logger_thread;

void* handle_temperature_sensor(void* ptr);
void* handle_gps_sensor(void* ptr);
void* handle_gps_pulse(void* ptr);
void* handle_actions(void* ptr);
void* handle_logger(void* ptr);

void toggle_lights();
void toggle_running();
int str_null_or_blank(char* str);
void init_work_space();
void clean_work_space();
void init_threads();
void destroy_threads();
char* get_nmea_field(char* nmea, int index);

pthread_rwlock_t isOn_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t temp_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t gps_rwlock = PTHREAD_RWLOCK_INITIALIZER;
pthread_rwlock_t nmea_rwlock = PTHREAD_RWLOCK_INITIALIZER;

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
  if (stat("./data", &std) == -1) {
    shell_print(BBLACK, "[THREAD%ld-MAIN]: Instantiating the 'data' Directory", main_thread);
    mkdir("./data", 0755);
  }
  // create the logs directory
  if (stat("./logs", &stl) == -1) {
    shell_print(BBLACK, "[THREAD%ld-MAIN]: Instantiating the 'logs' Directory", main_thread);
    mkdir("./logs", 0755);
  }
  csv_init();
  logger_init();

  // init signal handler for SIGUSR1, SIGINT, and SIGTSTP
  register_signal_handler(sig_handler);
  clean_up_gpio_pins();
  initialize_gpio_pins();
}
void clean_work_space() {
  uart_close();
  logger_close();
  cvs_close();
  destroy_threads();
}

/**
 * Initilizing 4 threads using Rate Monotonic Scheduling(RMS)
 * Thread 1 action_thread Priority 99
 * Thread 2 GPS_thread Priority 80
 * Thread 3 logger_thread Priority 70
 * Thread 4 temperature_thread Priority 50
 *
 */
void init_threads() {
  pthread_attr_t temp_attr, logger_attr, action_attr, gps_attr;
  struct sched_param temp_param, logger_param, action_param, gps_param;

  pthread_attr_init(&action_attr);
  pthread_attr_setschedpolicy(&action_attr, SCHED_FIFO);
  action_param.sched_priority = sched_get_priority_max(SCHED_FIFO);
  pthread_attr_setschedparam(&action_attr, &action_param);

  pthread_attr_init(&gps_attr);
  pthread_attr_setschedpolicy(&gps_attr, SCHED_FIFO);
  gps_param.sched_priority = 80;
  pthread_attr_setschedparam(&gps_attr, &gps_param);

  pthread_attr_init(&logger_attr);
  pthread_attr_setschedpolicy(&logger_attr, SCHED_FIFO);
  logger_param.sched_priority = 70;
  pthread_attr_setschedparam(&logger_attr, &logger_param);

  pthread_attr_init(&temp_attr);
  pthread_attr_setschedpolicy(&temp_attr, SCHED_FIFO);
  temp_param.sched_priority = 70;
  pthread_attr_setschedparam(&temp_attr, &temp_param);

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
void clean_up_gpio_pins() {
  gpio_unexport(ON_LIGHT);
  gpio_unexport(OFF_LIGHT);
  gpio_unexport(ON_OFF_BUTTON_PIN);
}
void unset_pins() {
  gpio_set_value(ON_LIGHT, LOW);
  gpio_set_value(OFF_LIGHT, LOW);
  gpio_set_value(ON_OFF_BUTTON_PIN, LOW);
}

/**
 * initialize_gpio_pins GPIO Pins
 */
void initialize_gpio_pins() {
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
 * updates every 2 seconds
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
    sleep(2);
  }
  return NULL;
}
/**
 * handle logic for the gps sensor
 */
void* handle_gps_sensor(void* ptr) {
  b_log(DEBUG, "[THREAD%ld-GPS]: Starting the GPS Thread...", gps_thread);
  uart_init(4);
  // Set GPS to only send GPRMC and GPGGA NMEA sentences
  serial_write(GPRMC_GPGGA);
  sleep(2);
  serial_flush();
  while (1) {
    char* nmea = serial_read_line();
    if (nmea != NULL && nmea[0] != '\0' && nmea[0] != '\n') {
      // b_log(DEBUG, "[THREAD%ld-GPS]: %s", gps_thread, nmea);
      // if NMEA is GPRMC
      if (strstr(nmea, "$GPRMC") != NULL) {
        pthread_rwlock_wrlock(&nmea_rwlock);
        memset(GPRMC_NMEA, 0, sizeof(char) * strlen(GPRMC_NMEA));
        strcpy(GPRMC_NMEA, nmea);
        pthread_rwlock_unlock(&nmea_rwlock);
        char* lat = get_nmea_field(nmea, 3);
        char* lon = get_nmea_field(nmea, 5);
        char* lat_d = get_nmea_field(nmea, 4);
        char* lon_d = get_nmea_field(nmea, 6);
        if (!str_null_or_blank(lat) && !str_null_or_blank(lon) && !str_null_or_blank(lat_d) && !str_null_or_blank(lon_d)) {
          pthread_rwlock_wrlock(&gps_rwlock);
          memset(latitude_str, 0, sizeof(char) * strlen(latitude_str));
          strcpy(latitude_str, lat);
          strcpy(longitude_str, lon);
          strcpy(latitude_hem, lat_d);
          strcpy(longitude_hem, lon_d);
          pthread_rwlock_unlock(&gps_rwlock);
        }
      } else if (strstr(nmea, "$GPGGA") != NULL) {
        pthread_rwlock_wrlock(&nmea_rwlock);
        memset(GPGGA_NMEA, 0, sizeof(char) * strlen(GPGGA_NMEA));
        strcpy(GPGGA_NMEA, nmea);
        pthread_rwlock_unlock(&nmea_rwlock);
        char* fix_str = get_nmea_field(nmea, 6);
        char* _number_of_satellites_str = get_nmea_field(nmea, 7);
        char* _altitude_str = get_nmea_field(nmea, 9);
        char* _altitude_unit = get_nmea_field(nmea, 10);
        if (!str_null_or_blank(fix_str) && !str_null_or_blank(_number_of_satellites_str) &&
            !str_null_or_blank(_altitude_str) && !str_null_or_blank(_altitude_unit)) {
          pthread_rwlock_wrlock(&gps_rwlock);
          strcpy(number_of_satellites_str, _number_of_satellites_str);
          strcpy(altitude_str, _altitude_str);
          strcpy(altitude_unit, _altitude_unit);
          fix = atoi(fix_str);
          pthread_rwlock_unlock(&gps_rwlock);
        }
      }
    }
    free(nmea);
  }
  return NULL;
}

/**
 * handle logic for the logging and writing to cvs file
 * update every 1 seconds
 */
void* handle_logger(void* ptr) {
  b_log(DEBUG, "[THREAD%ld-LOGGER]: Starting the LOGGER THREAD...", logger_thread);
  time_t base = time(0);
  time_t now = base;
  while (1) {
    pthread_rwlock_rdlock(&isOn_rwlock);
    // debug
    if (isOn) {
      pthread_rwlock_rdlock(&temp_rwlock);
      pthread_rwlock_unlock(&temp_rwlock);
      pthread_rwlock_rdlock(&gps_rwlock);
      // if GPS is working and its values are valid
      if (fix != 0) {
        b_log(INFO, "[THREAD%ld-LOGGER]: [latitude, longitude, alititude, stat, temp]: %s %s, %s %s, %s %s, %s, %.2f°C",
              logger_thread, latitude_str, latitude_hem, longitude_str, longitude_hem, altitude_str, altitude_unit,
              number_of_satellites_str, temp_c);
        //  atof(lat), atof(lon)
        log_csv("%s,%s,%s,%s,%s,%s,%s,%.2f", latitude_str, latitude_hem, longitude_str, longitude_hem, altitude_str,
                altitude_unit, number_of_satellites_str, temp_c);
      }
      now = time(0);
      // log gps pulse every 7 seconds
      if (now - base >= 7) {
        char gps_status[40] = {0};
        base = now;
        if (fix) {
          b_log(INFO, "[THREAD%ld-LOGGER]: SUCCESSFULLY GETTING GPS PULSE", logger_thread);
          pthread_rwlock_rdlock(&nmea_rwlock);
          b_log(DEBUG, "[THREAD%ld-LOGGER]: -NMEA- %s", logger_thread, GPGGA_NMEA);
          b_log(DEBUG, "[THREAD%ld-LOGGER]: -NMEA- %s", logger_thread, GPRMC_NMEA);
          pthread_rwlock_unlock(&nmea_rwlock);
        } else {
          b_log(WARN, "[THREAD%ld-LOGGER]: FAILING TO GET GPS PULSE", logger_thread);
          pthread_rwlock_rdlock(&nmea_rwlock);
          b_log(DEBUG, "[THREAD%ld-LOGGER]: -NMEA- %s", logger_thread, GPGGA_NMEA);
          b_log(DEBUG, "[THREAD%ld-LOGGER]: -NMEA- %s", logger_thread, GPRMC_NMEA);
          pthread_rwlock_unlock(&nmea_rwlock);
        }
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
 * update every 10 MS
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

/**
 * Get NMEA field
 * fields are seperated with comma
 */
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

/**
 * hellper function to check whether an string is null or blank
 */
int str_null_or_blank(char* str) {
  if (str == NULL || str[0] == '\0') {
    return 1;
  } else {
    return 0;
  }
}