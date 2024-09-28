/*
 * Michael Hylton.c
 *
 * sync.c
 *
 * Synchronization routines for SThread
 */

#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sched.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "sthread.h"
#include "sync.h"

/*
 * Atomic operations for x86 architecture.
 */
static inline int test_and_set_bit(volatile unsigned long *addr)
{
	int oldval;
	__asm__ __volatile__("xchgl %0, %1"
			: "=r"(oldval), "+m"(*(addr))	/* output */
			: "0"(1)						/* input */
			: "memory"	/* clobbered: changing contents of memory */
			);
	return oldval;
}
static inline void clear_bit(volatile unsigned long *addr)
{
	unsigned long oldval;
	__asm__ __volatile__("xchgl %0, %1"
			: "=r"(oldval), "+m"(*(addr))	/* output */
			: "0"(0)						/* input */
			: "memory"	/* clobbered: changing contents of memory */
			);
}

/*
 * rwlock routines
 */

int sthread_rwlock_init(sthread_rwlock_t *rwlock)
{
        printf("\nInitiate rwlock\n");
        rwlock->numOfReaders=0;
        rwlock->numOfWriters=0;
        return 0;
}

int sthread_rwlock_destroy(sthread_rwlock_t *rwlock)
{
    //reset all variables to their original conditions.
    printf("\nDestroy rwlock\n");
    rwlock->numOfReaders = 0; 
    rwlock->numOfWriters = 0;
    rwlock->queuedReaders = NULL; 
    rwlock->queuedWriters = NULL; 
    return 0;
}

int sthread_read_lock(sthread_rwlock_t *rwlock)
{
        //keeps the thread lock while writing
        printf("\nRead Lock\n");
        while(true){
          if (rwlock->numOfWriters ==0){ 
            rwlock->numOfReaders++; //initiate the lock
            return 0;
          }
          sthread_suspend(); // if the writer or reader is active, it suspeneds
        }
}

int sthread_read_try_lock(sthread_rwlock_t *rwlock)
{
        printf("\nTried Read\n");
        if(rwlock->numOfReaders==0){
          rwlock->numOfReaders++;
          return 0;
        }
        return -1;
}

int sthread_read_unlock(sthread_rwlock_t *rwlock)
{
        printf("\nread unlock\n");
        rwlock->numOfReaders--;
        //if no readers exist after decrementing, wakeup the next queued writers
        if (rwlock->numOfReaders ==0){ 
            if(rwlock->queuedWriters){
              sthread_wake(rwlock->queuedWriters); 
            }
          } 
        return 0;
}

int sthread_write_lock(sthread_rwlock_t *rwlock)
{
        printf("\nWrite lock\n");
        while(true){
          if (rwlock->numOfReaders ==0 && rwlock->numOfWriters==0){ 
            rwlock->numOfWriters++; //initiate the lock
            return 0;
          }
          sthread_suspend(); // if the writer is active, it suspeneds
        }
        
}

int sthread_write_try_lock(sthread_rwlock_t *rwlock)
{
        printf("\nTried write\n");
        if (rwlock->numOfReaders == 0 && rwlock->numOfWriters == 0) {
          rwlock->numOfWriters++;
          return 0;
        }
        return -1;
}

int sthread_write_unlock(sthread_rwlock_t *rwlock)
{
        printf("\nWrite unlock\n");
        rwlock->numOfWriters--;
        //if no readers exist after decrementing, wakeup the next queued writers or queued reader
        if (rwlock->numOfReaders ==0){ 
            sthread_wake(rwlock->queuedReaders); //first try the readers
            sthread_wake(rwlock->queuedWriters); //then try the writer
            
          } 
        return 0;
}

