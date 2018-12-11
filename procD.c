#include <stdio.h>
#include "queues.h"
#include "keys.h"
#include <time.h>	 /* nanosleep(), etc.                   */
#include <stdlib.h>      /* rand() and srand() functions        */
#include <unistd.h>	 /* fork(), etc.                        */

#define NUM_LOOPS 150

int main(void)
{
    struct timespec delay;      /* used for wasting time.               */
    int i;                      /* counter for loop operation.          */
    int cons;                   /* item consumed in consumer loop       */
    int num_of_BD_bus;         /* number of businessmen in the BD queue*/
    int num_of_CD_bus;         /* number of businessmen in the CD queue */
    int num_of_BD;
    int num_of_CD;

    int queue_BD_id;            /* id of BD queue */
    int queue_CD_id;            /* id of CD queue */

    struct Queue* Queue_BD;     /* pointer to the BD Queue */
    struct Queue* Queue_CD;     /* pointer to the CD Queue */

    /* Create BD queue and attach to it. */
    queue_BD_id = queue_create(queueBD_key);
    printf("Created queue of id: %d\n", queue_BD_id);
    Queue_BD = queue_attach(queue_BD_id);

    /* Create CD queue and attach to it. */
    queue_CD_id = queue_create(queueCD_key);
    printf("Created queue of id: %d\n", queue_CD_id);
    Queue_CD = queue_attach(queue_CD_id);

    /* Init semaphore for BD queue. */
    int sem_set_BD_id = sem_get_set_id(semBD_key, 3);
    printf("Created BD semaphore set of ID: %d\n", sem_set_BD_id);

    /* Init semaphore for CD queue. */
    int sem_set_CD_id = sem_get_set_id(semCD_key, 3);
    printf("Created CD semaphore set of ID: %d\n", sem_set_CD_id);

    /* Init semaphore for number of Businessmen in the BD queue */
    int sem_set_BDbus_id = sem_get_set_id(semBD_bus_key, 1);
    printf("Created BDbus semaphore set of ID: %d\n", sem_set_BDbus_id);

    /* Init semaphore for number of Businessmen in the CD queue */
    int sem_set_CDbus_id = sem_get_set_id(semCD_bus_key, 1);
    printf("Created CDbus semaphore set of ID: %d\n", sem_set_CDbus_id);

    for (int i=0; i<NUM_LOOPS; i++) {
        /* Get current value of businessmen in the BD and CD queue. */
        num_of_BD_bus = sem_get_value(sem_set_BDbus_id, 0);
        num_of_CD_bus = sem_get_value(sem_set_CDbus_id, 0);
        printf("Businessman in BD: %d\n", num_of_BD_bus);
        printf("Businessman in CD: %d\n", num_of_CD_bus);

        if (num_of_BD_bus != 0 || num_of_CD_bus != 0)
            /* If there is a businessmen in the queue. */
        {
            if(num_of_BD_bus > num_of_CD_bus)
            {
                /* Consume petent from BD queue if it has more businessmen
                 * in the queue than CD queue.
                 */
                /* Consumer for BD Queue */
                down(sem_set_BD_id, S2); /* down(s2) */
                down(sem_set_BD_id, MUTEX); /* down(mutex) */
                cons = dequeue(&Queue_BD); /* remove item from the queue */
                down(sem_set_BDbus_id, 0);
                up(sem_set_BD_id, MUTEX); /* up(mutex) */
                up(sem_set_BD_id, S1); /* up(s1) */
                printf("Iter: %d, Dequeued from BD: %d\n", i, cons); /*consume item*/
                fflush(stdout);
                
            } else {
                /* Consume petent from CD queue if it has more businessmen
                 * in the queue than BD queue.
                 */

                /* Consumer for CD Queue */
                down(sem_set_CD_id, S2); /* down(s2) */
                down(sem_set_CD_id, MUTEX); /* down(mutex) */
                cons = dequeue(&Queue_CD); /* remove item from the queue */
                down(sem_set_CDbus_id, 0);
                up(sem_set_CD_id, MUTEX); /* up(mutex) */
                up(sem_set_CD_id, S1); /* up(s1) */
                printf("Iter: %d, Dequeued from CD: %d\n", i, cons); /*consume item*/
                fflush(stdout);
            
            }
        }
        else {
            num_of_BD = sem_get_value(sem_set_BD_id, S2);
            if(num_of_BD != 0)
            {
                /* Consumer for BD Queue */
                down(sem_set_BD_id, S2); /* down(s2) */
                down(sem_set_BD_id, MUTEX); /* down(mutex) */
                cons = dequeue(&Queue_BD); /* remove item from the queue */
                up(sem_set_BD_id, MUTEX); /* up(mutex) */
                up(sem_set_BD_id, S1); /* up(s1) */
                printf("Iter: %d, Dequeued from BD: %d\n", i, cons); /*consume item*/
                fflush(stdout);
            }
            
            num_of_CD = sem_get_value(sem_set_CD_id, S2);
            if(num_of_CD != 0)
            {
                /* Consumer for CD Queue */
                down(sem_set_CD_id, S2); /* down(s2) */
                down(sem_set_CD_id, MUTEX); /* down(mutex) */
                cons = dequeue(&Queue_CD); /* remove item from the queue */
                up(sem_set_CD_id, MUTEX); /* up(mutex) */
                up(sem_set_CD_id, S1); /* up(s1) */
                printf("Iter: %d, Dequeued from CD: %d\n", i, cons); /*consume item*/
                fflush(stdout);
            }
                           
        }
        delay.tv_sec = 1;
        delay.tv_nsec = 10;
        nanosleep(&delay, NULL);
    }
    
    queue_detach(&Queue_BD);
    queue_delete(queue_BD_id);
    queue_detach(&Queue_CD);
    queue_delete(queue_CD_id);

    sem_delete(sem_set_BD_id,3);
    
    sem_delete(sem_set_CD_id,3);

    sem_delete(sem_set_BDbus_id, 1);
    sem_delete(sem_set_CDbus_id, 1);

    return 0;

}