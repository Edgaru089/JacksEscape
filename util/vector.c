
#include "vector.h"

#include "string.h"
#include "stdlib.h"
#include "assert.h"


vector_Vector *vector_Create(uintptr_t objectSize) {
	vector_Vector *vec = malloc(sizeof(vector_Vector));
	vec->objectSize    = objectSize;
	vec->size          = 0;
	vec->data          = 0;

	vec->data = malloc(objectSize);
	vec->cap  = objectSize;

	return vec;
}

// Resizes the underlying buffer to a new capacity
static inline void __vector_Rebuffer(vector_Vector *vec, uintptr_t newcap) {
	ASSERT(newcap >= vec->size);
	void *newbuf = malloc(newcap);
	memcpy(newbuf, vec->data, vec->size);
	free(vec->data);
	vec->data = newbuf;
	vec->cap  = newcap;
}

void vector_Destroy(vector_Vector *vec) {
	free(vec->data);
	free(vec);
}

void *vector_Push(vector_Vector *vec, const void *data) {
	vector_Append(vec, data, 1);
	return vec->data + vec->size - vec->objectSize;
}

bool vector_Pop(vector_Vector *vec, void *out_data) {
	if (!vector_Size(vec))
		return false;

	if (out_data)
		memcpy(out_data, vec->data + (vec->size - vec->objectSize), vec->objectSize);

	vector_Resize(vec, vector_Size(vec) - 1);

	return true;
}

void vector_Append(vector_Vector *vec, const void *data, uintptr_t n) {
	uintptr_t oldsize = vec->size, addsize = vec->objectSize * n;
	vector_Resize(vec, vector_Size(vec) + n);

	if (data)
		memcpy(vec->data + oldsize, data, addsize);
	else
		memset(vec->data + oldsize, 0, addsize);
}

void vector_Resize(vector_Vector *vec, uintptr_t size) {
	uintptr_t newsize = size * vec->objectSize;
	if (newsize > vec->cap) {
		// grow the buffer exponentially
		uint64_t newcap = vec->cap;
		while (newcap < newsize)
			newcap *= 2;
		__vector_Rebuffer(vec, newcap);
	}
	vec->size = newsize;
}

uintptr_t vector_Size(vector_Vector *vec) {
	return vec->size / vec->objectSize;
}

void vector_Clear(vector_Vector *vec) {
	vec->size = 0;
}

void vector_ShrinkBuffer(vector_Vector *vec) {
	if (vec->size != vec->cap)
		__vector_Rebuffer(vec, vec->size);
}

uintptr_t vector_Capacity(vector_Vector *vec) {
	return vec->cap / vec->objectSize;
}

void vector_Reserve(vector_Vector *vec, uintptr_t cap) {
	uintptr_t newcap = vec->objectSize * cap;
	if (newcap > vec->cap)
		__vector_Rebuffer(vec, newcap);
}

void *vector_At(vector_Vector *vec, uintptr_t i) {
	return vec->data + vec->objectSize * i;
}

// Data returns the data buffer.
void *vector_Data(vector_Vector *vec) {
	return vec->data;
}

void *vector_Back(vector_Vector *vec) {
	return vector_At(vec, vector_Size(vec) - 1);
}
