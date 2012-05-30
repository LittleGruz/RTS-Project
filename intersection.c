#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "timing.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

int main(void)
{
   mqd_t qd, qd2;
   char buf[MESSAGESIZE], buf2[MESSAGESIZE];
   char waiting[MESSAGESIZE];
   light_state states[8];
   light_state* current;
   int switcher, i;

   struct  mq_attr attr;
   attr.mq_maxmsg = 100;
   attr.mq_msgsize = MESSAGESIZE;
   attr.mq_flags = 0;
   
   init_states(states);
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
            /****** TIMED MODE ******/
            while(switcher){
               //mq_receive(qd, buf, MESSAGESIZE, NULL)
         
               if(i == 0){
                  strcpy(waiting,"\0");
      
                  // USER_INPUT
                  /* If multiple things waiting, input should still just be one
                     word e.g. nsc-t|ewpewc*/
                  printf("What is waiting? (t/nsc/ewc/nsp/ewp)(change):\n");
                  scanf("%s", waiting);
                  
                  // Check for waiting entities
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
                  if(strstr(waiting,"change") != NULL){
                     printf("Switching modes\n");
                     switcher = 0;
                  }
               }
               
               switcher = sensor_based(current, states, qd, i, waiting);
         
               if(i == 8)
                 i = 0;
               else
                 i++;
            }
            
            // Check if users wants to exit
            if(!strcmp(waiting, "done"))
               break;
            
            /****** END TIMED MODE ******/
            
            // Re-initialise values
            switcher = 1;
            i = 0;
            printf("Sensor mode active\n");
            
            /****** SENSOR MODE ******/
            while(switcher){
               strcpy(waiting,"\0");

               // USER_INPUT
               printf("What is waiting? (t/nsc/ewc/nsp/ewp)(change):\n");
               scanf("%s", waiting);
               
               // Check for waiting entities
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
               if(strstr(waiting,"change") != NULL){
                  printf("Switching modes\n");
                  switcher = 0;
               }
               
               switcher = sensor_based(current, states, qd, i, waiting);
               
               if(i == 8)
                  i = 0;
               else
                  i++;
            }
   
            // Check if users wants to exit
            if(!strcmp(waiting, "done"))
               break;
               
            /****** END SENSOR MODE ******/
            
            // Re-initialise values
            switcher = 1;
            i = 0;
            printf("Timer based mode active\n");
         }
         mq_close(qd);
         mq_close(qd2);
         mq_unlink("/root/comm2");
     }
   }
   return 0;
}
