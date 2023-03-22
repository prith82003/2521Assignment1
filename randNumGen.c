#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

int main(int argc, char **argv)
{
	int numTimes = rand() % 150;
	int maxInputNum = rand() % 1000;
	srand(time(NULL));

	if (argc > 1)
	{
		assert(argc == 3);
		assert(atoi(argv[1]) > 0);
		assert(atoi(argv[2]) > 0);

		numTimes = atoi(argv[1]);
		maxInputNum = atoi(argv[2]);
	}

	// int num1 = 1;
	// int num2 = 2;

	printf("+");
	for (int i = 0; i < numTimes; i++)
		printf(" %d", rand() % maxInputNum);

	printf("\nw\n");
}