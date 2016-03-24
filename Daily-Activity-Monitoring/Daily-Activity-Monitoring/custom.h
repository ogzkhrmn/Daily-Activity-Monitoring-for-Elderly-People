#pragma once
#include "stdafx.h"
#include <stdlib.h>

//This function allocates memory for arrays.
void allocateArray(int **array, int n, char *arrayName)
{
	*array = (int *)malloc(n * sizeof(int));
	if ((*array) == NULL)
	{
		printf("Couldn't allocate memory for %s!\n", arrayName);
		exit(-3);
	}
}

//This function allocated memory for matrices.
void allocate2D(int*** matrix, int row, int col, char *matrixName)
{
	int i;
	*matrix = (int **)malloc(row * sizeof(int *));
	if ((*matrix) == NULL)
	{
		printf("Couldn't allocate memory for %s!\n", matrixName);
		exit(-1);
	}
	for (i = 0; i<row; i++)
	{
		(*matrix)[i] = (int *)malloc(col * sizeof(int));
		if (((*matrix)[i]) == NULL)
		{
			printf("Couldn't allocate memory for %s's #%d row!\n", matrixName, i);
			exit(-2);
		}
	}
}
