/*
 *  mqsend.c
 *
 *  Simple example of using mqueue messaging between processes
 *
 *  Run mqsend in one terminal window, then after a few messages
 *  are queued (less than 5) run mqreceive in another terminal window
 *
*/


#include <stdio.h>
#include <mqueue.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include "mqtest.h"

#include "timing.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

int i;
char inter1[MESSAGESIZE], inter2[MESSAGESIZE], inter3[MESSAGESIZE];

int main(void)
{
    mqd_t qd;
    mqd_t qd1, qd2, qd3;
    int state;
    light_state states[8];
    light_state current;
    int switcher;

    struct  mq_attr  attr;

    init_states(states);

    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MESSAGESIZE;
    attr.mq_flags = 0;
    state = 0;
    current = states[0];
    i = 0;
   
   qd1 = mq_open("/root/comm1", O_RDONLY);
   qd2 = mq_open("/root/comm2", O_RDONLY);
   qd3 = mq_open("/root/comm3", O_RDONLY);

   mq_getattr(qd, &attr);

    qd = mq_open("/root/comm", Q_FLAGS, Q_PERM, &attr);
    if (qd != -1){
      while(1){

         while(switcher){

            switcher = timer_based(current, states, qd, inter2);
         
            if(i == 0){
               mq_send(qd, "trams?", MESSAGESIZE, 0);
               //strcpy should be deleted after threads are implemented
               strcpy(inter2,"\0");

               while(1){

                  //mq_send(qd, inter2, MESSAGESIZE, 0);
                  mq_receive(qd2, inter2, MESSAGESIZE, NULL);

                  if(strcmp(inter2,"y") == 0 || strcmp(inter2,"n") == 0)

                     break;

               }

            }
         
            if(i == 8)
               i = 0;
            else
               i++;
         }
         switcher = 1;
         i = 0;
         mq_send(qd, "Switched modes\n", MESSAGESIZE, 0);
         while(switcher){
            switcher = sensor_based(current, states, qd, inter2);
            
            if(i == 8)
               i = 0;
            else
               i++;
         
         mq_receive(qd1, inter1, MESSAGESIZE, NULL);
         mq_receive(qd2, inter2, MESSAGESIZE, NULL);
         mq_receive(qd3, inter3, MESSAGESIZE, NULL);
         }
         switcher = 1;
         i = 0;
         mq_send(qd, "Switched modes\n", MESSAGESIZE, 0);
      }
    }
    mq_close(qd);
    mq_close(qd1);
    mq_close(qd2);
    mq_close(qd3);
    mq_unlink("/root/comm");
    return 0;
}
