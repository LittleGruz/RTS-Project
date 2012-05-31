#include <stdio.h>
#include <mqueue.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include "timing.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

int main(void){
   mqd_t qdo1, qdo2, qdo3;
   mqd_t qd1, qd2, qd3;
   char inter1[MESSAGESIZE], inter2[MESSAGESIZE], inter3[MESSAGESIZE];
   char order1[MESSAGESIZE], order2[MESSAGESIZE], order3[MESSAGESIZE];

   struct  mq_attr  attr;
   
   // Communication from the intersections
   qd1 = mq_open("/root/comm1", O_RDONLY);
   qd2 = mq_open("/root/comm2", O_RDONLY);
   qd3 = mq_open("/root/comm3", O_RDONLY);


   // Communication to the intersections
   qdo1 = mq_open("/root/order1", Q_FLAGS, Q_PERM, &attr);
   qdo2 = mq_open("/root/order2", Q_FLAGS, Q_PERM, &attr);
   qdo3 = mq_open("/root/order3", Q_FLAGS, Q_PERM, &attr);
   
   if (qdo1 != -1 && qdo2 != -1 && qdo3 != -1){
      printf ("Intersection control started\n");
      // Just loop and print out the current state of each intersection
      while(1){
      
         mq_getattr(qd1, &attr);
         if(attr.mq_curmsgs > 0){
            mq_receive(qd1, inter1, MESSAGESIZE, NULL);
            printf("Intersection1: %s\n", inter1);
         }
         mq_getattr(qd2, &attr);
         if(attr.mq_curmsgs > 0){
            mq_receive(qd2, inter2, MESSAGESIZE, NULL);
            printf("Intersection2: %s\n", inter2);
         }
         mq_getattr(qd3, &attr);
         if(attr.mq_curmsgs > 0){
            mq_receive(qd3, inter3, MESSAGESIZE, NULL);
            printf("Intersection3: %s\n", inter3);
         }
         
         // Sending the overrides. Uncomment when ready. USER_INPUT
         /* Will continuously send the override command while the message
            contains 'or' and has no messages in the queue */
         mq_getattr(qd1, &attr);
         if(strstr(order1, "or") != NULL && attr.mq_curmsgs <= 0)
            mq_send(qdo1, order1, MESSAGESIZE, 0);
         mq_getattr(qd2, &attr);
         if(attr.mq_curmsgs <= 0){
            if(strstr(order2, "or") != NULL)
               mq_send(qdo2, order2, MESSAGESIZE, 0);
         }
         mq_getattr(qd3, &attr);
         if(strstr(order3, "or") != NULL && attr.mq_curmsgs <= 0)
            mq_send(qdo3, order3, MESSAGESIZE, 0);
         
         // USER_INPUT for exiting
         if(strcmp(inter1, "done") == 0 || strcmp(inter2, "done") == 0 || strcmp(inter3, "done") == 0)
            break;
      }
   }
   
   mq_close(qd1);
   mq_close(qd2);
   mq_close(qd3);
   mq_close(qdo1);
   mq_close(qdo2);
   mq_close(qdo3);
   mq_unlink("/root/comm");
   
   return 0;
}
