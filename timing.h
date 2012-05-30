#include <stdio.h>
#include <mqueue.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include "mqtest.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

typedef struct{
       int ns;
       int ew;
       int nsp;
       int ewp;
       int tram;
} light_state;

void init_states(light_state*);

int timer_based(light_state*, light_state*, mqd_t, int, char*);

int sensor_based(light_state*, light_state*, mqd_t, int, char*);