#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 65536

int main (int argc, const char* argv[])
{
	char buf[BUF_SIZE];
	int  size = 0;
	int  *variable_array = NULL;
	int  i;
	char *pch = NULL;
	char *_pch = NULL;
	char *temp = NULL;

	fgets(buf, BUF_SIZE, stdin);

	int  lengthofbuf = strlen(buf);
	pch = malloc(sizeof(char) * (lengthofbuf + 1));
	for (i = 0; i < lengthofbuf + 1; i++)
	{
		pch[i] = buf[i];
	}
	
	for (i = strlen(pch); i >= 0; i--)
	{
		if (pch[i] == '-')
		{
			pch[i - 1] = pch[strlen(pch) + 1];
			break;
		}
	}
	
	_pch = pch + strlen(pch) - 1;
	
	
	while (_pch != pch)
	{
		if (!isdigit(*(_pch)))
		{
			if (isdigit(*(_pch + 1)))
			{
				temp = _pch + 1;
				size++;
				variable_array = realloc(variable_array, sizeof(int) * size);
				variable_array[size - 1] = atoi(temp);
			}
			*_pch = '\0';
		}
		_pch--;
	}
	
	size++;
	variable_array = realloc(variable_array, sizeof(int) * size);
	variable_array[size - 1] = atoi(pch);

	for (i = 0; i < size; i++)
	{
		printf("%d ", variable_array[i]);
	}

	printf("\n");
	return 0;
}
