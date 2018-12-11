#include <stdio.h>       /* standard I/O routines.              */
#include <sys/types.h>   /* various type definitions.           */
#include <sys/ipc.h>     /* general SysV IPC structures         */
#include <sys/sem.h>	 /* semaphore functions and structs.    */
#include <sys/shm.h>     /* shared memory segment               */

#include "queues.h"

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif


/* Create a semaphore set with given number of semaphores in it.
 * If creating is successful returns 1.
 */
int sem_get_set_id(int set_id, int num_sem)
{
    int sem_set_id;
    /* Create sempahore set of given id and number of sempahores in it. */
    sem_set_id = semget(set_id, num_sem, IPC_CREAT | 0666);
    if (sem_set_id == -1) {
        perror("queues: semget");
        // exit(1);
    }
    return sem_set_id;
}

/* Delete semapore */
void sem_delete(int sem_set_id, int num_of_sem)
{
    union semun dummy; /* variable used to pass values to the semctl() call */
    int rc;              /* temporary variable used to check successful operation */

    for (int i = 0; i < num_of_sem; i++)
    {
            rc = semctl(sem_set_id, i, IPC_RMID, dummy);
        if (rc == -1) {
            perror("queues: semctl");   
            // exit(1);
    }
    }
    /* Set semphore of sem_id in sem_set_id to given value. */

}


/* Set value of sempahore in given semaphore set.
 * If operation is successful returns 1.
 */
void sem_set_value(int sem_set_id, int sem_id, int value)
{
    union semun sem_val; /* variable used to pass values to the semctl() call */
    int rc;              /* temporary variable used to check successful operation */

    /* Set semphore of sem_id in sem_set_id to given value. */
    sem_val.val = value;
    rc = semctl(sem_set_id, sem_id, SETVAL, sem_val);
    if (rc == -1) {
        perror("queues: semctl");   
        // exit(1);
    }
}



/* Get value of sempahore in given semaphore set.
 * If operation is successful returns 1.
 */
int sem_get_value(int sem_set_id, int sem_id)
{
    union semun sem_val; /* variable used to pass values to the semctl() call */
    int rc;              /* temporary variable used to check successful operation */

    /* Get semphore's value of sem_id in sem_set_id */
    rc = semctl(sem_set_id, sem_id, GETVAL);
    if (rc == -1) {
        perror("queues: semctl");
        // exit(1);
    }
    return rc;
}


/* Down operation on the semaphore. */
void down(int sem_set_id, int sem_id)
{
    struct sembuf sem_op;           /* structure for semaphore operations */

    sem_op.sem_num = sem_id;        /* semaphore number */
    sem_op.sem_op = -1;             /* semaphore operation */
    sem_op.sem_flg = 0;             /* operation flags */

    semop(sem_set_id, &sem_op, 1); 
}

/* Up operation on the semaphore. */
void up(int sem_set_id, int sem_id)
{
    struct sembuf sem_op;           /* structure for semaphore operations   */

    sem_op.sem_num = sem_id;        /* semaphore number */
    sem_op.sem_op = 1;              /* semaphore operation */
    sem_op.sem_flg = 0;             /* operation flags */

    semop(sem_set_id, &sem_op, 1);
}

/* Create a shared memory queue. Returns id of the queue. */
int queue_create(int queue_key)
{
    int queue_id = shmget(queue_key, sizeof(struct Queue), IPC_CREAT | 0775);
    return queue_id;
}


/* Attach to the shared memory segment of the queue. */
struct Queue* queue_attach(int queue_id)
{
    struct Queue* queue = (struct Queue*)shmat(queue_id, 0, 0);
    return queue;
}

/* Detach from the shared memory segment of the queue. */
void queue_detach(struct Queue** queue)
{
    shmdt(*queue); /* detach from queue */
}

/* Delete the shared memory segment */
void queue_delete(int queue_id)
{
    shmctl(queue_id, IPC_RMID, NULL);
}


/* Initialize values of Queue. */
void queue_init(struct Queue** queue)
{
    (*queue)->capacity = CAPACITY;
    (*queue)->front = (*queue)->size = 0;  
    (*queue)->rear = CAPACITY - 1;
}


/* Queue is full when size becomes equal to the capacity */
int isFull(struct Queue* queue) 
{  return (queue->size == queue->capacity);  } 
  
/* Queue is empty when size is 0 */
int isEmpty(struct Queue* queue) 
{  return (queue->size == 0); } 


/* Add item to the end of the queue */
void enqueue(struct Queue** queue, int item) 
{ 
    if (isFull(*queue)) 
    {
        perror("queues: enqueue");
        return; 
    }
    (*queue)->rear = ((*queue)->rear + 1)%(*queue)->capacity; 
    (*queue)->array[(*queue)->rear] = item; 
    (*queue)->size = (*queue)->size + 1; 
    // printf("%d enqueued to queue\n", item); 
    // printf("ENQUEUE: Current front: %d\n", queue->front);
    // printf("ENQUEUE: Current rear: %d\n", queue->rear);
    // printf("ENQUEUE: Current size: %d\n", queue->size);
} 


/* Add item to the frond of the queue */
void enqueue_front(struct Queue** queue, int item) 
{ 
    if (isFull(*queue)) 
    {
        perror("queues: enqueue");
        return; 
    }
    (*queue)->front = ((*queue)->front - 1)%(*queue)->capacity; 
    (*queue)->array[(*queue)->front] = item; 
    (*queue)->size = (*queue)->size + 1; 
    // printf("%d enqueued to queue\n", item); 
    // printf("ENQUEUE: Current front: %d\n", queue->front);
    // printf("ENQUEUE: Current rear: %d\n", queue->rear);
    // printf("ENQUEUE: Current size: %d\n", queue->size);
} 

/* Remove item from the queue */
int dequeue(struct Queue** queue) 
{ 
    if (isEmpty(*queue)) 
    {
        perror("queues: dequeue"); 
        return -1;
    }
    int item = (*queue)->array[(*queue)->front]; 
    (*queue)->front = ((*queue)->front + 1)%(*queue)->capacity; 
    (*queue)->size = (*queue)->size - 1; 
    // printf("DEQUEUE: Current front: %d\n", queue->front);
    // printf("DEQUEUE: Current rear: %d\n", queue->rear);
    // printf("DEQUEUE: Current size: %d\n", queue->size);
    return item;

    
}

/* Traverse through queue and print it's content */
void traverse(struct Queue** queue)
{
    int size_queue = (*queue)->size;
    int front = (*queue)->front;
    int rear = (*queue)->rear;
    int i = 0;
    // printf("-------------------------------------\n");
    // printf("EN(*queue): Current front: %d\n", (*queue)->front);
    // printf("EN(*queue): Current rear: %d\n", (*queue)->rear);
    // printf("EN(*queue): Current size: %d\n", (*queue)->size);
    // printf("About to enter while loop\n");
    while(size_queue != 0)
    {
        printf("%d\n", (*queue)->array[front]);
        
        if (front == 9)
        {
            front = 0;
        }else
        {
            front++;
        }

        size_queue--;
    }
    
} 