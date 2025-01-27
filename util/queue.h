#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
	uint8_t  *data;        // the data buffer
	uintptr_t size;        // size of data buffer
	uint8_t  *begin, *end; // begin and past-the-end for in-queue data
	uintptr_t count;       // number of in-queue bytes
} queue;

// initialize a queue with a existing buffer
void queue_InitBuffered(queue *q, void *buffer, uintptr_t size);

// writes one byte to the queue, discarding if full
void queue_PushByte(queue *q, const uint8_t b);

// pops one byte from the front of the queue, returning it
uint8_t queue_PopByte(queue *q);

// write Size bytes to the queue, none written if there is not space for all the bytes
void queue_Push(queue *q, const void *buffer, uintptr_t size);

// pops Size bytes from the queue, none popped if there are no enough data
// returns the number of bytes popped (either Size or 0)
uintptr_t queue_Pop(queue *q, void *buffer, uintptr_t size);

// return the byte at the front of the queue
uint8_t queue_FrontByte(queue *q);

// tells if the queue is empty
bool queue_Empty(queue *q);

// returns the number of bytes in the queue
uintptr_t queue_Size(queue *q);

// returns the empty space left at the end of the queue
uintptr_t queue_Space(queue *q);


#ifdef __cplusplus
}
#endif
