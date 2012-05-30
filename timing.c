#include <stdio.h>
#include <mqueue.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include "timing.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

int i;

// States initialisation: 0 = red; 1 = yellow/flashing; 2 = green;
light_state* init_states(light_state* states){
   states[0].ns = 0;
   states[0].ew = 0;
   states[0].nsp = 0;
   states[0].ewp = 0;
   states[0].tram = 0;

   states[1].ns = 0;
   states[1].ew = 2;
   states[1].nsp = 0;
   states[1].ewp = 2;
   states[1].tram = 0;

   states[2].ns = 0;
   states[2].ew = 2;
   states[2].nsp = 0;
   states[2].ewp = 1;
   states[2].tram = 0;

   states[3].ns = 0;
   states[3].ew = 1;
   states[3].nsp = 0;
   states[3].ewp = 0;
   states[3].tram = 0;

   // Tram state
   states[4].ns = 0;
   states[4].ew = 0;
   states[4].nsp = 0;
   states[4].ewp = 0;
   states[4].tram = 2;

   states[5].ns = 2;
   states[5].ew = 0;
   states[5].nsp = 2;
   states[5].ewp = 0;
   states[5].tram = 0;

   states[6].ns = 2;
   states[6].ew = 0;
   states[6].nsp = 1;
   states[6].ewp = 0;
   states[6].tram = 0;

   states[7].ns = 1;
   states[7].ew = 0;
   states[7].nsp = 0;
   states[7].ewp = 0;
   states[7].tram = 0;
   
   return states;
}

int timer_based(light_state current, light_state* states, mqd_t qd, char* waiting){
   
   // If input is to switch modes, then return false
   if(strstr("sensor",waiting) != NULL)
      return 0;
   
   // All the state switching and actions
   switch(i){
   case 0:
      current = states[0];
      mq_send(qd, "state0\n", MESSAGESIZE, 0);
      sleep(2);
      break;
   case 1:
      current = states[1];
      mq_send(qd, "state1\n", MESSAGESIZE, 0);
      sleep(3); //30
      break;
   case 2:
      current = states[2];
      mq_send(qd, "state2\n", MESSAGESIZE, 0);
      sleep(5);
      break;
   case 3:
      current = states[3];
      mq_send(qd, "state3\n", MESSAGESIZE, 0);
      sleep(3);
      break;
   case 4:
      current = states[0];
      mq_send(qd, "state4\n", MESSAGESIZE, 0);
      break;
   case 5:
      printf("Tram check | %s\n", waiting);
      if(strstr("t",waiting) != NULL){
        current = states[4];
        mq_send(qd, "state5\n", MESSAGESIZE, 0);
        sleep(2); //20
      }
      else
        sleep(2);
      break;
   case 6:
      current = states[5];
      mq_send(qd, "state6\n", MESSAGESIZE, 0);
      sleep(3); //30
      break;
   case 7:
      current = states[6];
      mq_send(qd, "state7\n", MESSAGESIZE, 0);
      sleep(5);
      break;
   case 8:
      current = states[7];
      mq_send(qd, "state8\n", MESSAGESIZE, 0);
      sleep(3);
   }

   return 1;
}

int sensor_based(light_state current, light_state* states, mqd_t qd, char* waiting){
   time_t seconds;
    char nscar[MESSAGESIZE], tram[MESSAGESIZE], ewcar[MESSAGESIZE];
   
   if(strcmp("y",nscar) != 0){
      printf("N-S Car? (y/n):\n");
      scanf("%s", nscar);
   }
   if(strcmp("y",ewcar) != 0){
      printf("E-W Car? (y/n):\n");
      scanf("%s", ewcar);
   }
   if(strcmp("y",tram) != 0){
      printf("Tram? (y/n):\n");
      scanf("%s", tram);
   }
   
   //If input is to switch modes, then return false
   if(strcmp("timed",nscar) == 0
      || strcmp("timed",ewcar) == 0
      || strcmp("timed",tram) == 0)
      return 0;
   
   switch(i){
   case 0:
      current = states[0];
      mq_send(qd, "state0\n", MESSAGESIZE, 0);
      sleep(2);
      //Save the time when the state will change
      seconds = time(NULL);
      break;
   case 1:
      current = states[1];
      mq_send(qd, "state1\n", MESSAGESIZE, 0);
      //Keep it looping until 30 at least seconds have passed and sensor is triggered
      if(time(NULL) - seconds >= 30
            && (strcmp("y",nscar) == 0 || strcmp("y",tram) == 0)){
         strcpy(tram,"n\0");
         strcpy(nscar,"n\0");
      }
      else 
      i--;
      break;
   case 2:
      current = states[2];
      mq_send(qd, "state2\n", MESSAGESIZE, 0);
      sleep(5);
      break;
   case 3:
      current = states[3];
      mq_send(qd, "state3\n", MESSAGESIZE, 0);
      sleep(3);
      break;
   case 4:
      current = states[0];
      mq_send(qd, "state4\n", MESSAGESIZE, 0);
      break;
   case 5:
      if(strstr("t",waiting) != NULL){
         current = states[4];
         mq_send(qd, "state5\n", MESSAGESIZE, 0);
         sleep(2);
      }
      else
         sleep(2);
      seconds = time(NULL);
      break;
   case 6:
      current = states[5];
      mq_send(qd, "state6\n", MESSAGESIZE, 0);
      //Keep it looping until 30 at least seconds have passed and sensor is triggered
      if(time(NULL) - seconds >= 30
         && strcmp("y",ewcar) == 0){
         strcpy(ewcar,"n\0");
      }
      else
         i--;
      break;
   case 7:
      current = states[6];
      mq_send(qd, "state7\n", MESSAGESIZE, 0);
      sleep(5);
      break;
   case 8:
      current = states[7];
      mq_send(qd, "state8\n", MESSAGESIZE, 0);
      sleep(3);
   }
   
   return 1;
}