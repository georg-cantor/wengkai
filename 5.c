#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 65536

int main (int argc, const char* argv[])
{
	char buf[BUF_SIZE];
	int  size = 0;
	int  element;
	int  *variable_array = NULL;
	int  i;
	char *pch = NULL;
	char *temp = NULL;

	fgets(buf, BUF_SIZE, stdin);

	for (i = strlen(buf) - 1; i >= 0; i--)
	{
		if (buf[i] == '-')
		{
			buf[i - 1] == '\0';
			break;
		}
	}

	
	pch = buf + strlen(buf);
	
	while (1)
	{
		if (!isdigit(*(pch)))
		{
			if (isdigit(*(pch + 1)))
			{
				temp = pch + 1;
				size++;
				realloc(variable_array, sizeof(int) * size);
				variable_array[size - 1] = atoi(temp);
			}
			*pch = '\0';
		}
		pch--;
	}

	for (i = 0; i < size; i++)
	{
		printf("variable_array[i] ");
	}

	printf("\n");

	return 0;
}