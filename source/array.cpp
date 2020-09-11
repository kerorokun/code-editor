#include "array.h"
#include "stdlib.h"

void
ArrayInit(Array *arr)
{
    arr->capacity = ARRAY_DEFAULT_CAPACITY;
    arr->len = 0;
    arr->items = (void **)malloc(ARRAY_DEFAULT_CAPACITY * sizeof(void *));
}

static void ArrayResize(Array *arr, int capacity)
{
    void **items = (void **)realloc(arr->items, capacity * sizeof(void *));
    if (items != NULL) {
	arr->items = items;
	arr->capacity = capacity;
    }
}

void
ArrayAdd(Array *arr, void *item)
{
    if (arr->capacity == arr->len) {
	ArrayResize(arr, arr->capacity * 2);
    }
    arr->items[arr->len] = item;
    arr->len++;
}

void
ArraySet(Array *arr, int index, void *item)
{
    if (index >= 0 && index < arr->len) {
	arr->items[index] = item;
    }
}

void *
ArrayGet(Array *arr, int index)
{
    if (!(index >= 0 && index < arr->len)) {
	return NULL;
    }
    return arr->items[index];
}


void
ArrayDelete(Array *arr, int index)
{
    if (index < 0 || index >= arr->len) {
	return;
    }

    arr->items[index] = NULL;

    for (int i = index; i < arr->len - 1; i++) {
	arr->items[i] = arr->items[i + 1];
	arr->items[i + 1] = NULL;
    }
    
    arr->len--;

    if (arr->len > 0 && arr->len == arr->capacity / 4) {
	ArrayResize(arr, arr->capacity / 2);
    }
}

void
ArrayFree(Array *arr)
{
    free(arr->items);
}

int
ArrayLength(Array *arr)
{
    return arr->len;
}
