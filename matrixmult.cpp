// Matrix multiplication timing test suite.
//
// (C) 2022 CubicleSoft.  All Rights Reserved.

#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS

#ifdef _MBCS
#undef _MBCS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// All implementations assume:
//   A_cols and B_rows are equal.
//   C is preallocated to be A_rows * B_cols in size.

// Implementation 1:  The most naive non-contiguous and contiguous implementations possible.
void MatrixMultiply_NonContiguous_1(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t j = 0; j < B_cols; j++)
		{
			C[i][j] = 0.0;

			for (size_t k = 0; k < A_cols; k++)
			{
				C[i][j] += A[i][k] * B[k][j];
			}
		}
	}
}

void MatrixMultiply_Contiguous_1(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t j = 0; j < B_cols; j++)
		{
			C[i * B_cols + j] = 0.0;

			for (size_t k = 0; k < A_cols; k++)
			{
				C[i * B_cols + j] += A[i * A_cols + k] * B[k * B_cols + j];
			}
		}
	}
}


// Implementation 2:  Temporary variable (possibly just a CPU register) vs. *(*(C + i * 8) + j * 8) or *(C + i * B_cols * 8 + j * 8) every loop.
void MatrixMultiply_NonContiguous_2(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpc;

	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t j = 0; j < B_cols; j++)
		{
			tmpc = 0.0;

			for (size_t k = 0; k < A_cols; k++)
			{
				tmpc += A[i][k] * B[k][j];
			}

			C[i][j] = tmpc;
		}
	}
}

void MatrixMultiply_Contiguous_2(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpc;

	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t j = 0; j < B_cols; j++)
		{
			tmpc = 0.0;

			for (size_t k = 0; k < A_cols; k++)
			{
				tmpc += A[i * A_cols + k] * B[k * B_cols + j];
			}

			C[i * B_cols + j] = tmpc;
		}
	}
}


// Implementation 3:  Swap j and k loops.  Variant of Implementation 1.
void MatrixMultiply_NonContiguous_3(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t k = 0; k < A_cols; k++)
		{
			for (size_t j = 0; j < B_cols; j++)
			{
				if (k)  C[i][j] += A[i][k] * B[k][j];
				else  C[i][j] = A[i][k] * B[k][j];
			}
		}
	}
}

void MatrixMultiply_Contiguous_3(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t k = 0; k < A_cols; k++)
		{
			for (size_t j = 0; j < B_cols; j++)
			{
				if (k)  C[i * B_cols + j] += A[i * A_cols + k] * B[k * B_cols + j];
				else  C[i * B_cols + j] = A[i * A_cols + k] * B[k * B_cols + j];
			}
		}
	}
}


// Implementation 4:  Temporary variable for A[i][k].  Variant of Implementation 3.
void MatrixMultiply_NonContiguous_4(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t k = 0; k < A_cols; k++)
		{
			tmpa = A[i][k];

			for (size_t j = 0; j < B_cols; j++)
			{
				if (k)  C[i][j] += tmpa * B[k][j];
				else  C[i][j] = tmpa * B[k][j];
			}
		}
	}
}

void MatrixMultiply_Contiguous_4(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t k = 0; k < A_cols; k++)
		{
			tmpa = A[i * A_cols + k];

			for (size_t j = 0; j < B_cols; j++)
			{
				if (k)  C[i * B_cols + j] += tmpa * B[k * B_cols + j];
				else  C[i * B_cols + j] = tmpa * B[k * B_cols + j];
			}
		}
	}
}


// Implementation 5:  Move conditional outside the innermost for loop.  Variant of Implementation 4.
void MatrixMultiply_NonContiguous_5(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t k = 0; k < A_cols; k++)
		{
			tmpa = A[i][k];

			if (!k)
			{
				for (size_t j = 0; j < B_cols; j++)
				{
					C[i][j] = tmpa * B[k][j];
				}
			}
			else
			{
				for (size_t j = 0; j < B_cols; j++)
				{
					C[i][j] += tmpa * B[k][j];
				}
			}
		}
	}
}

void MatrixMultiply_Contiguous_5(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		for (size_t k = 0; k < A_cols; k++)
		{
			tmpa = A[i * A_cols + k];

			if (!k)
			{
				for (size_t j = 0; j < B_cols; j++)
				{
					C[i * B_cols + j] = tmpa * B[k * B_cols + j];
				}
			}
			else
			{
				for (size_t j = 0; j < B_cols; j++)
				{
					C[i * B_cols + j] += tmpa * B[k * B_cols + j];
				}
			}
		}
	}
}


// Implementation 6:  Remove conditional altogether.  Variant of Implementation 5.
// This implementation arguably offers the best balance between code length, code readability, and performance.
// That is, both functions are short, easy to understand, and faster than the naive implementation.
void MatrixMultiply_NonContiguous_6(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		tmpa = A[i][0];

		for (size_t j = 0; j < B_cols; j++)
		{
			C[i][j] = tmpa * B[0][j];
		}

		for (size_t k = 1; k < A_cols; k++)
		{
			tmpa = A[i][k];

			for (size_t j = 0; j < B_cols; j++)
			{
				C[i][j] += tmpa * B[k][j];
			}
		}
	}
}

void MatrixMultiply_Contiguous_6(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		tmpa = A[i * A_cols];

		for (size_t j = 0; j < B_cols; j++)
		{
			C[i * B_cols + j] = tmpa * B[j];
		}

		for (size_t k = 1; k < A_cols; k++)
		{
			tmpa = A[i * A_cols + k];

			for (size_t j = 0; j < B_cols; j++)
			{
				C[i * B_cols + j] += tmpa * B[k * B_cols + j];
			}
		}
	}
}


// Implementation 7:  Precalculate base row start for the contiguous implementation.  Non-contiguous is the same as Implementation 6.  Variant of Implementation 6.
void MatrixMultiply_NonContiguous_7(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	for (size_t i = 0; i < A_rows; i++)
	{
		tmpa = A[i][0];

		for (size_t j = 0; j < B_cols; j++)
		{
			C[i][j] = tmpa * B[0][j];
		}

		for (size_t k = 1; k < A_cols; k++)
		{
			tmpa = A[i][k];

			for (size_t j = 0; j < B_cols; j++)
			{
				C[i][j] += tmpa * B[k][j];
			}
		}
	}
}

void MatrixMultiply_Contiguous_7(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	size_t starta, startb, startc;
	double tmpa;

	starta = 0;
	startc = 0;
	for (size_t i = 0; i < A_rows; i++)
	{
		tmpa = A[starta];

		for (size_t j = 0; j < B_cols; j++)
		{
			C[startc + j] = tmpa * B[j];
		}

		starta++;
		startb = B_cols;

		for (size_t k = 1; k < A_cols; k++)
		{
			tmpa = A[starta];

			for (size_t j = 0; j < B_cols; j++)
			{
				C[startc + j] += tmpa * B[startb + j];
			}

			starta++;
			startb += B_cols;
		}

		startc += B_cols;
	}
}


// Implementation 8:  Convert to use simple pointer math to completely remove all array access multiplication.  Variant of Implementation 6.
void MatrixMultiply_NonContiguous_8(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa, *aptr, **bbase, *bptr, *cptr, *cptr2;

	for (size_t i = 0; i < A_rows; i++)
	{
		aptr = *A;
		tmpa = *aptr;
		bbase = B;
		bptr = *bbase;
		cptr = *C;

		cptr2 = cptr;

		for (size_t j = 0; j < B_cols; j++)
		{
			*cptr2 = tmpa * *bptr;

			bptr++;
			cptr2++;
		}

		for (size_t k = 1; k < A_cols; k++)
		{
			aptr++;
			tmpa = *aptr;
			bbase++;
			bptr = *bbase;
			cptr2 = cptr;

			for (size_t j = 0; j < B_cols; j++)
			{
				*cptr2 += tmpa * *bptr;

				bptr++;
				cptr2++;
			}
		}

		A++;
		C++;
	}
}

void MatrixMultiply_Contiguous_8(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa, *bptr, *cptr, *cptr2;

	cptr = C;
	cptr2 = C;
	for (size_t i = 0; i < A_rows; i++)
	{
		tmpa = *A;
		bptr = B;

		for (size_t j = 0; j < B_cols; j++)
		{
			*cptr2 = tmpa * *bptr;

			bptr++;
			cptr2++;
		}

		A++;

		for (size_t k = 1; k < A_cols; k++)
		{
			tmpa = *A;
			cptr2 = cptr;

			for (size_t j = 0; j < B_cols; j++)
			{
				*cptr2 += tmpa * *bptr;

				bptr++;
				cptr2++;
			}

			A++;
		}

		cptr = cptr2;
	}
}


// Implementation 9:  Optimize larger arrays for CPU cache lines via a blocking algorithm.  Variant of Implementation 6.
//
// Relevant paper:  https://netlib.org/utk/papers/autoblock/node2.html
// Additional optimization:  Due to swapping j and k loops, the blocking i loop from the paper can be skipped.

// 3 * b * b data referenced.
#define MATRIX_STEP  24

void MatrixMultiply_NonContiguous_9(double **C, double **A, double **B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	// A_cols == B_rows.
	if (A_cols > MATRIX_STEP || B_cols > MATRIX_STEP)
	{
		for (size_t i = 0; i < A_rows; i++)
		{
			for (size_t j = 0; j < B_cols; j++)
			{
				C[i][j] = 0;
			}

			for (size_t k = 0; k < A_cols; k += MATRIX_STEP)
			{
				size_t k2max = (k + MATRIX_STEP <= A_cols ? k + MATRIX_STEP : A_cols);

				for (size_t j = 0; j < B_cols; j += MATRIX_STEP)
				{
					size_t j2max = (j + MATRIX_STEP <= B_cols ? j + MATRIX_STEP : B_cols);

					for (size_t k2 = k; k2 < k2max; k2++)
					{
						tmpa = A[i][k2];

						for (size_t j2 = j; j2 < j2max; j2++)
						{
							C[i][j2] += tmpa * B[k2][j2];
						}
					}
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < A_rows; i++)
		{
			tmpa = A[i][0];

			for (size_t j = 0; j < B_cols; j++)
			{
				C[i][j] = tmpa * B[0][j];
			}

			for (size_t k = 1; k < A_cols; k++)
			{
				tmpa = A[i][k];

				for (size_t j = 0; j < B_cols; j++)
				{
					C[i][j] += tmpa * B[k][j];
				}
			}
		}
	}
}

void MatrixMultiply_Contiguous_9(double *C, double *A, double *B, size_t A_rows, size_t A_cols, size_t B_cols)
{
	double tmpa;

	// A_cols == B_rows.
	if (A_cols > MATRIX_STEP || B_cols > MATRIX_STEP)
	{
		for (size_t i = 0; i < A_rows; i++)
		{
			for (size_t j = 0; j < B_cols; j++)
			{
				C[i * B_cols + j] = 0;
			}

			for (size_t k = 0; k < A_cols; k += MATRIX_STEP)
			{
				size_t k2max = (k + MATRIX_STEP <= A_cols ? k + MATRIX_STEP : A_cols);

				for (size_t j = 0; j < B_cols; j += MATRIX_STEP)
				{
					size_t j2max = (j + MATRIX_STEP <= B_cols ? j + MATRIX_STEP : B_cols);

					for (size_t k2 = k; k2 < k2max; k2++)
					{
						tmpa = A[i * A_cols + k2];

						for (size_t j2 = j; j2 < j2max; j2++)
						{
							C[i * B_cols + j2] += tmpa * B[k2 * B_cols + j2];
						}
					}
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < A_rows; i++)
		{
			tmpa = A[i * A_cols];

			for (size_t j = 0; j < B_cols; j++)
			{
				C[i * B_cols + j] = tmpa * B[j];
			}

			for (size_t k = 1; k < A_cols; k++)
			{
				tmpa = A[i * A_cols + k];

				for (size_t j = 0; j < B_cols; j++)
				{
					C[i * B_cols + j] += tmpa * B[k * B_cols + j];
				}
			}
		}
	}
}

#undef MATRIX_STEP


double **CreateNonContiguousMatrix(size_t rows, size_t cols)
{
	double **result, *row, num = 1.0;

	result = (double **)malloc(sizeof(double *) * rows);
	if (result == NULL)  return NULL;

	for (size_t y = 0; y < rows; y++)
	{
		row = (double *)malloc(sizeof(double) * cols);
		if (row == NULL)  return NULL;

		for (size_t x = 0; x < cols; x++)
		{
			row[x] = num;

			num += 1.0;
		}

		result[y] = row;
	}

	return result;
}

double *CreateContiguousMatrix(size_t rows, size_t cols)
{
	double *result, num = 1.0;

	result = (double *)malloc(sizeof(double) * rows * cols);

	for (size_t y = 0; y < rows; y++)
	{
		for (size_t x = 0; x < cols; x++)
		{
			result[y * cols + x] = num;

			num += 1.0;
		}
	}

	return result;
}

void DumpSyntax(char *currfile)
{
	printf("(C) 2022 CubicleSoft.  All Rights Reserved.\n\n");

	printf("Syntax:  %s mode [options]\n\n", currfile);

	printf("Modes:\n\
\n\
\tvalidate A_rows [A_cols B_cols]\n\
\tValidates all implementations against the naive implementation.\n\
\tWhen A_cols and B_cols are omitted, A_rows is used for all dimensions.\n\
\n\
\tbench c/n implementation_num rounds A_rows [A_cols B_cols]\n\
\tRuns the Implementation number for the specified number of rounds.\n\
\tc/n runs the c = contiguous OR n = non-contiguous version.\n\
\tWhen A_cols and B_cols are omitted, A_rows is used for all dimensions.\n\n");
}

int main(int argc, char **argv)
{
	size_t A_rows, A_cols, B_cols, funcnum, rounds;
	double **A_ptrs, **B_ptrs, **C_ptrs, **C_ptrs2;
	double *A, *B, *C, *C2;
	bool valid, contiguous;
	struct timespec ts, ts2;

	void (*noncontiguousfuncs[9])(double **, double **, double **, size_t, size_t, size_t) = {
		MatrixMultiply_NonContiguous_1, MatrixMultiply_NonContiguous_2, MatrixMultiply_NonContiguous_3,
		MatrixMultiply_NonContiguous_4, MatrixMultiply_NonContiguous_5, MatrixMultiply_NonContiguous_6,
		MatrixMultiply_NonContiguous_7, MatrixMultiply_NonContiguous_8, MatrixMultiply_NonContiguous_9
	};

	void (*contiguousfuncs[9])(double *, double *, double *, size_t, size_t, size_t) = {
		MatrixMultiply_Contiguous_1, MatrixMultiply_Contiguous_2, MatrixMultiply_Contiguous_3,
		MatrixMultiply_Contiguous_4, MatrixMultiply_Contiguous_5, MatrixMultiply_Contiguous_6,
		MatrixMultiply_Contiguous_7, MatrixMultiply_Contiguous_8, MatrixMultiply_Contiguous_9
	};

	if (argc > 2 && !strcmp(argv[1], "validate"))
	{
		A_rows = (size_t)atoi(argv[2]);

		if (argc < 5)
		{
			A_cols = A_rows;
			B_cols = A_rows;
		}
		else
		{
			A_cols = (size_t)atoi(argv[3]);
			B_cols = (size_t)atoi(argv[4]);
		}

		A_ptrs = CreateNonContiguousMatrix(A_rows, A_cols);
		B_ptrs = CreateNonContiguousMatrix(A_cols, B_cols);
		C_ptrs = CreateNonContiguousMatrix(A_rows, B_cols);
		C_ptrs2 = CreateNonContiguousMatrix(A_rows, B_cols);

		A = CreateContiguousMatrix(A_rows, A_cols);
		B = CreateContiguousMatrix(A_cols, B_cols);
		C = CreateContiguousMatrix(A_rows, B_cols);
		C2 = CreateContiguousMatrix(A_rows, B_cols);

		if (A_ptrs == NULL || B_ptrs == NULL || C_ptrs == NULL || C_ptrs2 == NULL || A == NULL || B == NULL || C == NULL || C2 == NULL)
		{
			printf("Memory allocation failed.\n");

			return 1;
		}

		// Run the naive calculation.
		MatrixMultiply_NonContiguous_1(C_ptrs2, A_ptrs, B_ptrs, A_rows, A_cols, B_cols);
		MatrixMultiply_Contiguous_1(C2, A, B, A_rows, A_cols, B_cols);

		// Validate the other Implementations.
		for (funcnum = 1; funcnum < (sizeof(noncontiguousfuncs) / sizeof(*noncontiguousfuncs)); funcnum++)
		{
			valid = true;

			noncontiguousfuncs[funcnum](C_ptrs, A_ptrs, B_ptrs, A_rows, A_cols, B_cols);

			for (size_t y = 0; y < A_rows; y++)
			{
				for (size_t x = 0; x < B_cols; x++)
				{
					double diff = C_ptrs[y][x] - C_ptrs2[y][x];

					if (diff < -0.001 || diff > 0.001)  valid = false;
				}
			}

			if (valid)  printf("Non-contiguous Implementation %u is VALID.\n", (unsigned int)(funcnum + 1));
			else  printf("Non-contiguous Implementation %u is INVALID.\n", (unsigned int)(funcnum + 1));
		}

		for (funcnum = 1; funcnum < (sizeof(contiguousfuncs) / sizeof(*contiguousfuncs)); funcnum++)
		{
			valid = true;

			contiguousfuncs[funcnum](C, A, B, A_rows, A_cols, B_cols);

			for (size_t y = 0; y < A_rows; y++)
			{
				for (size_t x = 0; x < B_cols; x++)
				{
					double diff = C[y * B_cols + x] - C2[y * B_cols + x];

					if (diff < -0.001 || diff > 0.001)  valid = false;
				}
			}

			if (valid)  printf("Contiguous Implementation %u is VALID.\n", (unsigned int)(funcnum + 1));
			else  printf("Contiguous Implementation %u is INVALID.\n", (unsigned int)(funcnum + 1));
		}
	}
	else if (argc > 5 && !strcmp(argv[1], "bench"))
	{
		contiguous = (argv[2][0] == 'c');

		funcnum = (size_t)atoi(argv[3]) - 1;

		if (contiguous && funcnum >= (sizeof(contiguousfuncs) / sizeof(*contiguousfuncs)))
		{
			printf("Contiguous Implementation %u does not exist.  Valid range 1-%u.\n", (unsigned int)(funcnum + 1), (unsigned int)(sizeof(contiguousfuncs) / sizeof(*contiguousfuncs)));

			return 1;
		}

		if (!contiguous && funcnum >= (sizeof(noncontiguousfuncs) / sizeof(*noncontiguousfuncs)))
		{
			printf("Non-contiguous Implementation %u does not exist.  Valid range 1-%u.\n", (unsigned int)(funcnum + 1), (unsigned int)(sizeof(noncontiguousfuncs) / sizeof(*noncontiguousfuncs)));

			return 1;
		}

		rounds = (size_t)atoi(argv[4]);

		A_rows = (size_t)atoi(argv[5]);

		if (argc < 8)
		{
			A_cols = A_rows;
			B_cols = A_rows;
		}
		else
		{
			A_cols = (size_t)atoi(argv[6]);
			B_cols = (size_t)atoi(argv[7]);
		}

		if (contiguous)
		{
			A = CreateContiguousMatrix(A_rows, A_cols);
			B = CreateContiguousMatrix(A_cols, B_cols);
			C = CreateContiguousMatrix(A_rows, B_cols);

			if (A == NULL || B == NULL || C == NULL)
			{
				printf("Memory allocation failed.\n");

				return 1;
			}
		}
		else
		{
			A_ptrs = CreateNonContiguousMatrix(A_rows, A_cols);
			B_ptrs = CreateNonContiguousMatrix(A_cols, B_cols);
			C_ptrs = CreateNonContiguousMatrix(A_rows, B_cols);

			if (A_ptrs == NULL || B_ptrs == NULL || C_ptrs == NULL)
			{
				printf("Memory allocation failed.\n");

				return 1;
			}
		}

		timespec_get(&ts, TIME_UTC);

		if (contiguous)
		{
			while (rounds)
			{
				contiguousfuncs[funcnum](C, A, B, A_rows, A_cols, B_cols);

				rounds--;
			}
		}
		else
		{
			while (rounds)
			{
				noncontiguousfuncs[funcnum](C_ptrs, A_ptrs, B_ptrs, A_rows, A_cols, B_cols);

				rounds--;
			}
		}

		timespec_get(&ts2, TIME_UTC);

		printf("%lf\n", (double)(ts2.tv_sec - ts.tv_sec) + (double)(ts2.tv_nsec - ts.tv_nsec) / 1000000000.0);
	}
	else
	{
		DumpSyntax(argv[0]);

		return 1;
	}

	return 0;
}
