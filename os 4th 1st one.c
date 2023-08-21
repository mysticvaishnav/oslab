
USING SEMAPHORE
#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<errno.h>
#include <stdlib.h>
#include<sched.h>
int philNo[5] = { 0, 1, 2, 3, 4 };
// my_semaphore structure 
typedef struct {
// Semaphore mutual exclusion variable 
pthread_mutex_t mutex;
// Semaphore count variable 
int cnt;
// Semaphore conditonal variable 
pthread_cond_t conditional_variable;
}
my_semaphore;
// Function to initialise the semaphore variables
int init(my_semaphore *sema, int pshared, int val) {
// The case when pshared == 1 is not implemeted as it was not required because the 
philosphers are implemented using threads and not processes.
lO M oA R cP S D| 2 81 1 40 20
if(pshared == 1){
printf("Cannot handle semaphores shared between processes!!! Exiting\n");
return -1;
}
// Initialisng the semaphore conditonal variable 
pthread_cond_init(&sema->conditional_variable, NULL);
// Initialisng the semaphore count variable 
sema->cnt = val;
// Initialisng the semaphore mutual exclusion variable 
pthread_mutex_init(&sema->mutex, NULL);
return 0;
}
int signal(my_semaphore *sema) {
//This locks the mutex so that only thread can access the critical section at a time 
pthread_mutex_lock(&sema->mutex);
sema->cnt = sema->cnt + 1;
// This wakes up one waiting thread 
if (sema->cnt)
pthread_cond_signal(&sema->conditional_variable);
lO M oA R cP S D| 2 81 1 40 20
// A woken thread must acquire the lock, so it will also have to wait until we call unlock
// This releases the mutex 
pthread_mutex_unlock(&sema->mutex);
return 0;
}
int wait(my_semaphore *sema) {
//This locks the mutex so that only thread can access the critical section at a time 
pthread_mutex_lock(&sema->mutex);
// While the semaphore count variable value is 0 the mutex is blocked on the conditon 
variable
while (!(sema->cnt))
pthread_cond_wait(&sema->conditional_variable, &sema->mutex);
// unlock mutex, wait, relock mutex 
sema->cnt = sema->cnt - 1;
// This releases the mutex and threads can access mutex
pthread_mutex_unlock(&sema->mutex);
return 0;
}
// Print semaphore value for debugging
void signal1(my_semaphore *sema) {
printf("Semaphore variable value = %d\n", sema->cnt);
}
// Declaring the semaphore variables which are the shared resources by the threads 
my_semaphore forks[5], bowls;
//Function for the philospher threads to eat 
void *eat_food(void *arg) {
while(1) {
int* i = arg;
// This puts a wait condition on the bowls to be used by the current philospher so 
that the philospher can access these forks whenever they are free
wait(&bowls);
lO M oA R cP S D| 2 81 1 40 20
// This puts a wait condition on the forks to be used by the current philospher so 
that the philospher can access these forks whenever they are free
wait(&forks[*i]); 
wait(&forks[(*i+4)%5]); 
sleep(1);
//Print the philospher number, its thread ID and the number of the forks it uses for
eating printf("Philosopher %d with ID %ld eats using forks %d and %d\n", *i+1, 
pthread_self(), *i+1, (*i+4)%5+1);
// This signals the other philospher threads that the bowls are available for 
eating signal(&bowls);
// This signals the other philospher threads that these forks are available for 
eating and thus other threads are woken up
signal(&forks[*i]);
signal(&forks[(*i+4)%5]);
sched_yield();
}
}
void main() {
int i = 0;
// Initialising the forks (shared variable) semaphores
while(i < 5){
init(&forks[i], 0, 1);
i++;
}
// Initialising the bowl (shared variable) semaphore 
init(&bowls, 0, 1);
// Declaring the philospher threads 
pthread_t phil[5];
i = 0;
// Creating the philospher threads 
while(i < 5) {
pthread_create(&phil[i], NULL, eat_food, &philNo[i]);
lO M oA R cP S D| 2 81 1 40 20
i++;
}
i = 0;
// Waits for all the threads to end their execution before ending
while(i < 5) {
pthread_join(phil[i], NULL);
i++;
}