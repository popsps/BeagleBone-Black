#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <gps.h>

// usage gcc 08-gps.c -o 08-gps -lgps -lm
int main(int argc, char* argv[]) {
    gps_init();
    loc_t gps;
    gps_location(&gps);
    printf("The boad locatio is (%lf, %lf)\n", gps.latitude, gps.longitude);
    return 0;
}