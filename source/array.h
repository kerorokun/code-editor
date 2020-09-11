#ifndef ARRAY_H
#define ARRAY_H

#define ARRAY_DEFAULT_CAPACITY 4

/**
 * Dynamically growing array to store arbitrary data
 */
typedef struct {
    void **items;
    size_t len;
    size_t capacity;
} Array;

/* Initialize an array */
void ArrayInit(Array *arr);

/* Add an element*/
void ArrayAdd(Array *arr, void *item);

/* Set an element's value */
void ArraySet(Array *arr, int index, void *item);

/* Get the current element */
void * ArrayGet(Array *arr, int index);


void ArrayDelete(Array *arr, int index);
void ArrayFree(Array *arr);
int ArrayLength(Array *arr);
static void ArrayResize(Array *arr, int capacity);
    
#endif
