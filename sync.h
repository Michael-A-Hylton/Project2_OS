/*
 * NAME, etc.
 *
 * sync.h
 */

#ifndef _STHREAD_SYNC_H_
#define _STHREAD_SYNC_H_
#include "Sthread.h"

struct sthread_rwlock_struct {
	int numOfReaders;
	int numOfWriters;
	sthread_t QueuedReaders;
	sthread_t QueuedWriters;
}sthread_rwlock_t;

typedef struct sthread_rwlock_struct sthread_rwlock_t;

int sthread_rwlock_init(sthread_rwlock_t *rwlock);
int sthread_rwlock_destroy(sthread_rwlock_t *rwlock);
int sthread_read_lock(sthread_rwlock_t *rwlock);
int sthread_read_try_lock(sthread_rwlock_t *rwlock);
int sthread_read_unlock(sthread_rwlock_t *rwlock);
int sthread_write_lock(sthread_rwlock_t *rwlock);
int sthread_write_try_lock(sthread_rwlock_t *rwlock);
int sthread_write_unlock(sthread_rwlock_t *rwlock);

#endif
