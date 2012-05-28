/*
 *  mqreceive.c
 *
 *  Simple example of using mqueue messaging between processes
 *
 *  Run mqsend in one terminal window, then after a few messages
 *  are queued (less than 5) run mqreceive in another terminal window
 *
*/


#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "mqtest.h"

#define Q_FLAGS  O_RDWR | O_CREAT
#define Q_PERM   S_IRUSR | S_IWUSR | S_IROTH

int main (void)
{
    mqd_t   qd, qd2;
    char    buf[MESSAGESIZE], buf2[MESSAGESIZE];

    struct  mq_attr attr;
    attr.mq_maxmsg = 100;
    attr.mq_msgsize = MESSAGESIZE;
    attr.mq_flags = 0;

    if ((qd = mq_open("/root/wang", O_RDONLY)) != -1)
    {
		if((qd2 = mq_open("/root/wang3", Q_FLAGS, Q_PERM, &attr)) != 1){
			mq_getattr(qd, &attr);
			printf ("max. %u msgs, %u bytes; waiting: %u\n",
					attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
			while (mq_receive(qd, buf, MESSAGESIZE, NULL) > 0)
			{
				if(strcmp(buf,"trams?") == 0){
					printf("Is a tram waiting? (y/n):\n");
					scanf("%s", buf2);
					mq_send(qd2, buf2, MESSAGESIZE, 0);
					continue;
				}
				printf("%s\n", buf);
				if (!strcmp(buf, "done"))
					break;
			}
			mq_close(qd);
			mq_close(qd2);
		}
    }
    return 0;
}
