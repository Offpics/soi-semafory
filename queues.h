struct Queue 
{ 
    int front, rear, size; 
    unsigned capacity; 
    int array[10]; 
}; 

#define CAPACITY 10

int sem_get_set_id(int set_id, int num_sem);
void sem_set_value(int sem_set_id, int sem_id, int value);
int sem_get_value(int sem_set_id, int sem_id);
void down(int sem_set_id, int sem_id);
void up(int sem_set_id, int sem_id);
int queue_create(int queue_key);
struct Queue* queue_attach(int queue_id);
void queue_detach(struct Queue** queue);
void queue_delete(int queue_id);
void enqueue(struct Queue** queue, int item);
int dequeue(struct Queue** queue);
void traverse(struct Queue** queue);
void enqueue_front(struct Queue** queue, int item);
void queue_init(struct Queue** queue);
void sem_delete(int sem_set_id, int num_of_sem);