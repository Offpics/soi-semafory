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

    int queue_AB_id;            /* id of AB queue */
    int queue_BD_id;            /* id of BD queue */

    struct Queue* Queue_AB;     /* pointer to the AB Queue */
    struct Queue* Queue_BD;     /* pointer to the BD Queue */


    /* Create AB queue and attach to it. */
    queue_AB_id = queue_create(queueAB_key);
    printf("Created queue of id: %d\n", queue_AB_id);
    Queue_AB = queue_attach(queue_AB_id);

    /* Create BD queue, attach to it and init its values. */
    queue_BD_id = queue_create(queueBD_key);
    printf("Created queue of id: %d\n", queue_BD_id);
    Queue_BD = queue_attach(queue_BD_id);
    queue_init(&Queue_BD);

    /* Init semaphore for AB queue. */
    int sem_set_AB_id = sem_get_set_id(semAB_key, 3);
    printf("Created AB semaphore set of ID: %d\n", sem_set_AB_id);

    /* Init semaphores for BD queue */
    int sem_set_BD_id = sem_get_set_id(semBD_key, 3);
    sem_set_value(sem_set_BD_id, 0, 1); /* set mutex to 1 */
    sem_set_value(sem_set_BD_id, 1, CAPACITY); /* set s1 to 10 */
    sem_set_value(sem_set_BD_id, 2, 0); /* set s2 to 0 */
    printf("Created BD semaphore set of ID: %d\n", sem_set_BD_id);

    /* Init semaphore for number of Businessmen in the BD queue */
    int sem_set_BDbus_key = sem_get_set_id(semBD_bus_key, 1);
    printf("Created BDbus semaphore set of ID: %d\n", sem_set_BDbus_key);
    sem_set_value(sem_set_BDbus_key, 0, 0); /* set sem value to 0 */

    srand(time(NULL));

    for (int i=0; i<NUM_LOOPS; i++) {
        /* Consumer for AB Queue. */
        down(sem_set_AB_id, S2); /* down(s2) */
        down(sem_set_AB_id, MUTEX); /* down(mutex) */
        cons = dequeue(&Queue_AB); /* remove item from the queue */
        up(sem_set_AB_id, MUTEX); /* up(mutex) */
        up(sem_set_AB_id, S1); /* up(s1) */
        printf("Iter: %d, Dequeued from AB: %d\n", i, cons); /*consume item*/
        fflush(stdout);

        /* Producent for BD Queue. */
        down(sem_set_BD_id, S1); /* down(s1) */
        down(sem_set_BD_id, MUTEX); /* down(mutex) */ 
        if(cons == 0 )
            enqueue(&Queue_BD, cons); /* add private person to the end of the queue */
        else if(cons == 1)
        {
            enqueue_front(&Queue_BD, cons); /* add businessman to the frond of the queue */
            up(sem_set_BDbus_key, 0);       /* increase the number of buss in the queue */
        }
        up(sem_set_BD_id, MUTEX); /* up(mutex) */
        up(sem_set_BD_id, S2); /* up(s2) */
        printf("Iter: %d, Enqueued to BD: %d\n", i, cons); /*consume item*/
        fflush(stdout);

        delay.tv_sec = 1;
        delay.tv_nsec = 10;
        nanosleep(&delay, NULL);
    }


    
    queue_detach(&Queue_AB);
    queue_delete(queue_AB_id);
    queue_detach(&Queue_BD);
    queue_delete(queue_BD_id);


    sem_delete(sem_set_AB_id,3);

    return 0;

}