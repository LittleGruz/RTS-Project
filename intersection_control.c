#include <stdio.h>
#include <mqueue.h>
#include <time.h>
#include <sys/stat.h>
#include <string.h>
#include "timing.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

char inter1[MESSAGESIZE], inter2[MESSAGESIZE], inter3[MESSAGESIZE];

int main(void)
{
   mqd_t qd;
   mqd_t qd1, qd2, qd3;

   struct  mq_attr  attr;

   attr.mq_maxmsg = 100;
   attr.mq_msgsize = MESSAGESIZE;
   attr.mq_flags = 0;
   
   qd1 = mq_open("/root/comm1", O_RDONLY);
   qd2 = mq_open("/root/comm2", O_RDONLY);
   qd3 = mq_open("/root/comm3", O_RDONLY);

   mq_getattr(qd, &attr);

   qd = mq_open("/root/comm", Q_FLAGS, Q_PERM, &attr);
   if (qd != -1){
      // Just loop and print out the current state of each intersection
      while(1){
         if(mq_receive(qd1, inter1, MESSAGESIZE, NULL) > 0){
            printf("Intersection1: %s\n", inter1);
         }
         if(mq_receive(qd2, inter2, MESSAGESIZE, NULL) > 0){
            printf("Intersection2: %s\n", inter2);
         }
         if(mq_receive(qd3, inter3, MESSAGESIZE, NULL) > 0){
            printf("Intersection3: %s\n", inter3);
         }
         // Change this when threads get added
         if(strcmp(inter1, "done") == 0 || strcmp(inter2, "done") == 0 || strcmp(inter3, "done") == 0)
            break;
      }
   }
   mq_close(qd);
   mq_close(qd1);
   mq_close(qd2);
   mq_close(qd3);
   mq_unlink("/root/comm");
   return 0;
}
