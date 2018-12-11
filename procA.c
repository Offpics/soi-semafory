#include <stdio.h>
#include "queues.h"
#include "keys.h"
#include <time.h>	     /* nanosleep(), etc.                   */
#include <stdlib.h>      /* rand() and srand() functions        */
#include <unistd.h>	     /* fork(), etc.                        */

#define NUM_LOOPS 100

int main(void)
{
    struct timespec delay;      /* used for wasting time.         */
    int i;                      /* counter for loop operation.    */
    int petent1;                /* petent to the queue AB         */
    int petent2;                /* petent to the queue AC         */

    int queue_AB_id;            /* id of AB queue */
    int queue_AC_id;            /* id of AC queue */

    struct Queue* Queue_AB;     /* pointer to the AB Queue */
    struct Queue* Queue_AC;     /* pointer to the AC Queue */


    /* Create AB queue, attach to it and init its values. */
    queue_AB_id = queue_create(queueAB_key);
    printf("Created queue of id: %d\n", queue_AB_id);
    Queue_AB = queue_attach(queue_AB_id);
    queue_init(&Queue_AB);

    /* Create AC queue, attach to it and init its values. */
    queue_AC_id = queue_create(queueAC_key);
    printf("Created queue of id: %d\n", queue_AC_id);
    Queue_AC = queue_attach(queue_AC_id);
    queue_init(&Queue_AC);

    /* Init semaphores for AB queue */
    int sem_set_AB_id = sem_get_set_id(semAB_key, 3);
    sem_set_value(sem_set_AB_id, 0, 1); /* set mutex to 1 */
    sem_set_value(sem_set_AB_id, 1, CAPACITY); /* set s1 to 10 */
    sem_set_value(sem_set_AB_id, 2, 0); /* set s2 to 0 */
    printf("Created AB semaphore set of ID: %d\n", sem_set_AB_id);

    /* Init semaphores for AC queues. */
    int sem_set_AC_id = sem_get_set_id(semAC_key, 3);
    sem_set_value(sem_set_AC_id, 0, 1); /* set mutex to 1 */
    sem_set_value(sem_set_AC_id, 1, CAPACITY); /* set s1 to 10 */
    sem_set_value(sem_set_AC_id, 2, 0); /* set s2 to 0 */
    printf("Created AC semaphore set of ID: %d\n", sem_set_AC_id);

    /* Init semaphore for printf. */
    int sem_printf = sem_get_set_id(sem_procA_print_key, 1);
    sem_set_value(sem_printf, 0, 1); /* set mutex to 1 */


    int child_pid;	      /* PID of child process */
    child_pid = fork();   /* create child process */
    switch (child_pid) {
	case -1:	/* fork() failed */
	    perror("fork");
	    exit(1);
	case 0:		/* Producer for AB queue */
	    for (int i=0; i<NUM_LOOPS; i++) {
        petent1 = rand() % 2;
        down(sem_set_AB_id, S1); /* down(s1) */
        down(sem_set_AB_id, MUTEX); /* down(mutex) */
        enqueue(&Queue_AB, petent1); /* add petent to the queue */
        up(sem_set_AB_id, MUTEX); /* up(mutex) */
        up(sem_set_AB_id, S2); /* up(s2) */

        /* Print information */
        down(sem_printf, MUTEX);
        printf("Iter: %d, Enqueued to AB: %d\n", i, petent1); /*consume item*/
        fflush(stdout);
        up(sem_printf, MUTEX);

        delay.tv_sec = 1;
        delay.tv_nsec = 10;
        nanosleep(&delay, NULL);
        }
	    break;
	default:	/* Producer for AC queue */
		for (int i=0; i<NUM_LOOPS; i++) {
        petent2 = rand() % 2;
        down(sem_set_AC_id, S1); /* down(s1) */
        down(sem_set_AC_id, MUTEX); /* down(mutex) */
        enqueue(&Queue_AC, petent2); /* add petent to the queue */
        up(sem_set_AC_id, MUTEX); /* up(mutex) */
        up(sem_set_AC_id, S2); /* up(s2) */

        /* Print information */
        down(sem_printf, MUTEX);
        printf("Iter: %d, Enqueued to AC: %d\n", i, petent2); /*consume item*/
        fflush(stdout);
        up(sem_printf, MUTEX);

        
        delay.tv_sec = 1;
        delay.tv_nsec = 10;
        nanosleep(&delay, NULL);
	    
        }
	    break;
    }



    
    queue_detach(&Queue_AB);
    queue_delete(queue_AB_id);
    queue_detach(&Queue_AC);
    queue_delete(queue_AC_id);

    sem_delete(sem_printf, 1);

    
    return 0;
}