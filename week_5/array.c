#include "array.h"
#include <stdlib.h>
#include <stdio.h>

Array array_create(int init_size)
{
	Array a;
	a.size = init_size;
	a.array = (int*)malloc(sizeof(int) * a.size);
	return a;
}
void  array_free(Array* a)
{
	free(a->array);
	a->array = NULL;
	a->size = 0;
}
int   array_size(const Array *a)
{
	return a->size;
}
int*  array_at(Array *a, int index)
{
	return &(a->array[index]);
}

int   array_get(const Array *a, int index)
{
	return a->array[index];
}
void  array_set(Array *a, int index, int value)
{
	a->array[index] = value;
}

void  array_inflate(Array *a, int more_size)
{
	
}






