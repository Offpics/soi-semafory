#include <stdio.h>
#include "queues.h"
#include "keys.h"
#include <time.h>	 /* nanosleep(), etc.                   */
#include <stdlib.h>      /* rand() and srand() functions        */
#include <unistd.h>	 /* fork(), etc.                        */

#define NUM_LOOPS 100

int main(void)
{
    struct timespec delay;      /* used for wasting time.         */
    int i;                      /* counter for loop operation.    */
    int cons;                   /* item consumed in consumer loop */

    int queue_AC_id;            /* id of AC queue */
    int queue_CD_id;            /* id of CD queue */

    struct Queue* Queue_AC;     /* pointer to the AC Queue */
    struct Queue* Queue_CD;     /* pointer to the CD Queue */

    /* Create AC queue and attach to it. */
    queue_AC_id = queue_create(queueAC_key);
    printf("Created queue of id: %d\n", queue_AC_id);
    Queue_AC = queue_attach(queue_AC_id);

    /* Create CD queue, attach to it and init its values. */
    queue_CD_id = queue_create(queueCD_key);
    printf("Created queue of id: %d\n", queue_CD_id);
    Queue_CD = queue_attach(queue_CD_id);
    queue_init(&Queue_CD);

    /* Init semaphores for AC queue. */
    int sem_set_AC_id = sem_get_set_id(semAC_key, 3);
    printf("Created AC semaphore set of ID: %d\n", sem_set_AC_id);

    /* Init semaphores for CD queue. */
    int sem_set_CD_id = sem_get_set_id(semCD_key, 3);
    sem_set_value(sem_set_CD_id, 0, 1); /* set mutex to 1 */
    sem_set_value(sem_set_CD_id, 1, CAPACITY); /* set s1 to 10 */
    sem_set_value(sem_set_CD_id, 2, 0); /* set s2 to 0 */
    printf("Created CD semaphore set of ID: %d\n", sem_set_CD_id);

     /* Init semaphores for number of Businessmen in the CD queue. */
    int sem_set_CDbus_id = sem_get_set_id(semCD_bus_key, 1);
    printf("Created CDbus semaphore set of ID: %d\n", sem_set_CDbus_id);
    sem_set_value(sem_set_CDbus_id, 0, 0); /* set bussiness to 0 */

    srand(time(NULL));

    for (int i=0; i<NUM_LOOPS; i++) {
        /* Consumer for AC Queue */
        down(sem_set_AC_id, 2); /* down(s2) */
        down(sem_set_AC_id, 0); /* down(mutex) */
        cons = dequeue(&Queue_AC); /* remove item from the queue */
        up(sem_set_AC_id, 0); /* up(mutex) */
        up(sem_set_AC_id, 1); /* up(s1) */
        printf("Iter: %d, Dequeued from AC: %d\n", i, cons); /*consume item*/
        fflush(stdout);

        /* Producent for CD queue */
        down(sem_set_CD_id, S1); /* down(s1) */
        down(sem_set_CD_id, MUTEX); /* down(mutex) */
        if(cons == 0 )
            enqueue(&Queue_CD, cons); /* add private person to the end of the queueu */
        else if (cons == 1)
        {
            enqueue_front(&Queue_CD, cons); /* add businessman to the frond of the queue */
            up(sem_set_CDbus_id, 0);        /* increase the number of buss in the queue */
        }
        up(sem_set_CD_id, MUTEX); /* up(mutex) */
        up(sem_set_CD_id, S2); /* up(s2) */
        printf("Iter: %d, Enqueued to CD: %d\n", i, cons); /*consume item*/
        fflush(stdout);

        delay.tv_sec = 1;
        delay.tv_nsec = 10;
        nanosleep(&delay, NULL);
    }


    
    queue_detach(&Queue_AC);
    queue_delete(queue_AC_id);
    queue_detach(&Queue_CD);
    queue_delete(queue_CD_id);

    sem_delete(sem_set_AC_id,3);


    return 0;

}