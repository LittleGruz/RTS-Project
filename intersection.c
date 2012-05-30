#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "mqtest.h"
#include "timing.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

int main(void)
{
   mqd_t qd, qd2;
   char buf[MESSAGESIZE], buf2[MESSAGESIZE];
   char waiting[MESSAGESIZE];
   light_state states[8];
   light_state current;
   int switcher, i;

   struct  mq_attr attr;
   attr.mq_maxmsg = 100;
   attr.mq_msgsize = MESSAGESIZE;
   attr.mq_flags = 0;
   
   states = init_states(states);
   i = 0;
   switcher = 1;

   if ((qd = mq_open("/root/comm", O_RDONLY)) != -1)
   {
      if((qd2 = mq_open("/root/comm2", Q_FLAGS, Q_PERM, &attr)) != 1){
         mq_getattr(qd, &attr);
         printf ("max. %u msgs, %u bytes; waiting: %u\n",
            attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
         while (1)
         {
            while(switcher){

               switcher = timer_based(current, states, qd2, waiting);
               //mq_receive(qd, buf, MESSAGESIZE, NULL)
         
               if(i == 0){
                  strcpy(waiting,"\0");
      
                  //Change all these if conditions after threads are implemented
                  printf("What is waiting? (t/nsc/ewc/nsp/ewp):\n");
                  scanf("%s", waiting);
                  if(strstr(waiting,"t") != NULL){
                     printf("Tram waiting\n");
                  }
                  if(strstr(waiting,"nsc") != NULL){
                     printf("NS car waiting\n");
                  }
                  if(strstr(waiting,"ewc") != NULL){
                     printf("EW car waiting\n");
                  }
                  if(strstr(waiting,"nsp") != NULL){
                     printf("NS pedestrian waiting\n");
                  }
                  if(strstr(waiting,"ewp") != NULL){
                     printf("EW pedestrian waiting\n");
                  }
                  if(strstr(waiting,"sensor") != NULL){
                     printf("Switching modes\n");
                     switcher = 0;
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
               switcher = sensor_based(current, states, qd, waiting);
            
               if(i == 8)
                  i = 0;
               else
                  i++;
            }
            switcher = 1;
            i = 0;
            mq_send(qd, "Switched modes\n", MESSAGESIZE, 0);
   
   
            printf("%s\n", buf);
            if(!strcmp(buf, "done"))
               break;
         }
       mq_close(qd);
       mq_close(qd2);
     }
   }
   return 0;
}
