#include <stdio.h>

void swap(int*, int*);

int main (int argc, const char *argv[])
{
	int x,  y;
	x = 5;
	y = 10;
	swap(&x, &y);
	printf("x = %d, y = %d\n", x, y);
	return 0;
}

void swap(int* a, int* b)
{
	int temp;
	
	temp = *a;
	*a = *b;
	*b = temp;
}
