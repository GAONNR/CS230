/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */


#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  int bi, bj, i, j, k, tmp;
  int x, y, bs = 16;
  int t_0, t_1, t_2, t_3, t_4, t_5, t_6, t_7;

  if (M == 32 && N == 32){
      for (bi = 0; bi < 32; bi += 8){
	      for (bj = 0; bj < 32; bj += 8){
		      for (i = bi; i < bi + 8; i++){
			      if (bi == bj) tmp  = A[i][i];
			      for (j = bj; j < bj + 8; j++){
				      if (i != j)
				      	B[j][i] = A[i][j];
			      }
			      if (bi == bj) B[i][i] = tmp;
		      }
	      }
      }
  }

  else if (M == 64 && N == 64){
    for (i = 0; i < 64; i += 8){
  		for (j = 0; j < 64; j += 8){
  			for (k = 0; k < 4; k++){
  				t_0 = A[j + k][i + 0];
  				t_1 = A[j + k][i + 1];
  				t_2 = A[j + k][i + 2];
  				t_3 = A[j + k][i + 3];
  				t_4 = A[j + k][i + 4];
  				t_5 = A[j + k][i + 5];
  				t_6 = A[j + k][i + 6];
  				t_7 = A[j + k][i + 7];

  				B[i + 0][j + k] = t_0;
  				B[i + 0][j + k + 4] = t_4;
  				B[i + 1][j + k] = t_1;
  				B[i + 1][j + k + 4] = t_5;
  				B[i + 2][j + k] = t_2;
  				B[i + 2][j + k + 4] = t_6;
  				B[i + 3][j + k] = t_3;
  				B[i + 3][j + k + 4] = t_7;
  			}

  			for (k = 0; k < 4; k++){
  				t_0 = B[i + k][j + 4];
  				t_1 = B[i + k][j + 5];
  				t_2 = B[i + k][j + 6];
  				t_3 = B[i + k][j + 7];

  				B[i + k][j + 4] = A[j + 4][i + k];
  				B[i + k][j + 5] = A[j + 5][i + k];
  				B[i + k][j + 6] = A[j + 6][i + k];
  				B[i + k][j + 7] = A[j + 7][i + k];

  				B[i + 4 + k][j + 0] = t_0;
  				B[i + 4 + k][j + 1] = t_1;
  				B[i + 4 + k][j + 2] = t_2;
  				B[i + 4 + k][j + 3] = t_3;
  			}

  			for (k = 0; k < 4; k++){
  				B[i + 4 + k][j + 4] = A[j + 4][i + 4 + k];
  				B[i + 4 + k][j + 5] = A[j + 5][i + 4 + k];
  				B[i + 4 + k][j + 6] = A[j + 6][i + 4 + k];
  				B[i + 4 + k][j + 7] = A[j + 7][i + 4 + k];
  			}
  		}
  	}
  }

  else if (M == 61 && N == 67){
	  for (bi = 0; bi < N; bi += bs){
		  if (bi + bs >= N) x = N;
		  else x = bi + bs;
		  for (bj = 0; bj < M; bj += bs){
			  if (bj + bs >= M) y = M;
			  else y = bj + bs;
			  for(i = bi; i < x; i++){
				  for (j = bj; j < y; j++){
					  B[j][i] = A[i][j];
				  }
			  }
		  }
	  }
  }

}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);

}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
