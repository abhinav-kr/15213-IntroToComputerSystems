/*
 * Abhinav Kuruvadi
 * Andrew-ID : akuruvad
 *
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
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */

void transpose3232(int M, int N, int A[N][M], int B[M][N]) {
    
    int blocki = 0, blockj = 0, i = 0, j = 0;
    
    int buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8;
    
    for ( blocki = 0 ; blocki < M; blocki+= 8 ) {
        for (blockj = 0 ; blockj < N; blockj+=8 ) {
            
            if ( blocki != blockj ) {
                for ( i = 0 ; i < 8; i++) {
                    for (j = 0; j < 8 ; j++) {
                        B[blockj+i][blocki+j] = A[blocki+j][blockj+i];
                    }
                }
            } else {
                for (i = blocki; i < blocki + 8; ++i) {
                    buf1 = A[i][blockj];
                    buf2 = A[i][blockj+1];
                    buf3 = A[i][blockj+2];
                    buf4 = A[i][blockj+3];
                    buf5 = A[i][blockj+4];
                    buf6 = A[i][blockj+5];
                    buf7 = A[i][blockj+6];
                    buf8 = A[i][blockj+7];
                    
                    B[blockj][i] = buf1;
                    B[blockj+1][i] = buf2;
                    B[blockj+2][i] = buf3;
                    B[blockj+3][i] = buf4;
                    B[blockj+4][i] = buf5;
                    B[blockj+5][i] = buf6;
                    B[blockj+6][i] = buf7;
                    B[blockj+7][i] = buf8;
                }
            }
        }
    }
}

void transpose6464(int M, int N, int A[N][M], int B[M][N]) {
    
    int blocki = 0, blockj = 0, i = 0, j = 0;
    
    int buf1, buf2, buf3, buf4, buf5, buf6, buf7, buf8;
    
    for ( blocki = 0 ; blocki < M; blocki+= 8 ) {
        for (blockj = 0 ; blockj < N; blockj+= 8 ) {
            
            if ( blocki != blockj ) {
                for ( i = 0 ; i < 4 ; i++ ) {
                    buf1 = A[i+blocki][blockj+0];
                    buf2 = A[i+blocki][blockj+1];
                    buf3 = A[i+blocki][blockj+2];
                    buf4 = A[i+blocki][blockj+3];
                    buf5 = A[i+blocki][blockj+4];
                    buf6 = A[i+blocki][blockj+5];
                    buf7 = A[i+blocki][blockj+6];
                    buf8 = A[i+blocki][blockj+7];
                    
                    B[i+blockj][blocki+0] = buf1;
                    B[i+blockj][blocki+1] = buf2;
                    B[i+blockj][blocki+2] = buf3;
                    B[i+blockj][blocki+3] = buf4;
                    B[i+blockj][blocki+4] = buf5;
                    B[i+blockj][blocki+5] = buf6;
                    B[i+blockj][blocki+6] = buf7;
                    B[i+blockj][blocki+7] = buf8;
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+i][blocki+j];
                        B[blockj+i][blocki+j] = B[blockj+j][blocki+i];
                        B[blockj+j][blocki+i] = buf1;
                    }
                }
                
                //The idea is to load the right memory into cache such that we can carry out a whole lot of required operations with the loaded data.
                
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    
                    buf5 = B[blockj+i][blocki+4+0];
                    buf6 = B[blockj+i][blocki+4+1];
                    buf7 = B[blockj+i][blocki+4+2];
                    buf8 = B[blockj+i][blocki+4+3];
                    buf1 = A[blocki+4+i][blockj+0];
                    buf2 = A[blocki+4+i][blockj+1];
                    buf3 = A[blocki+4+i][blockj+2];
                    buf4 = A[blocki+4+i][blockj+3];
                    
                    B[blockj+i][blocki+4+0] = buf1;
                    B[blockj+i][blocki+4+1] = buf2;
                    B[blockj+i][blocki+4+2] = buf3;
                    B[blockj+i][blocki+4+3] = buf4;
                    B[blockj+4+i][blocki+0] = buf5;
                    B[blockj+4+i][blocki+1] = buf6;
                    B[blockj+4+i][blocki+2] = buf7;
                    B[blockj+4+i][blocki+3] = buf8;
                    
                    buf5 = A[blocki+4+i][blockj+4+0];
                    buf6 = A[blocki+4+i][blockj+4+1];
                    buf7 = A[blocki+4+i][blockj+4+2];
                    buf8 = A[blocki+4+i][blockj+4+3];
                    B[blockj+4+i][blocki+4+0] = buf5;
                    B[blockj+4+i][blocki+4+1] = buf6;
                    B[blockj+4+i][blocki+4+2] = buf7;
                    B[blockj+4+i][blocki+4+3] = buf8;
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+i+4][blocki+j+4];
                        B[blockj+i+4][blocki+j+4] = B[blockj+j+4][blocki+i+4];
                        B[blockj+j+4][blocki+i+4] = buf1;
                    }
                }
                
                for ( i = 0; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i; j++) {
                        buf1 = B[blockj+i+4][blocki+j];
                        B[blockj+i+4][blocki+j] = B[blockj+j+4][blocki+i];
                        B[blockj+j+4][blocki+i] = buf1;
                    }
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+i][blocki+j+4];
                        B[blockj+i][blocki+j+4] = B[blockj+j][blocki+i+4];
                        B[blockj+j][blocki+i+4] = buf1;
                    }
                }
                
            } else if ( blocki == blockj ) {
                
                for ( i = blocki ; i < blocki + 4; i++ ) {
                    buf1 = A[i][blockj+0];
                    buf2 = A[i][blockj+1];
                    buf3 = A[i][blockj+2];
                    buf4 = A[i][blockj+3];
                    buf5 = A[i][blockj+4];
                    buf6 = A[i][blockj+5];
                    buf7 = A[i][blockj+6];
                    buf8 = A[i][blockj+7];
                    
                    B[i][blockj+0] = buf1;
                    B[i][blockj+1] = buf2;
                    B[i][blockj+2] = buf3;
                    B[i][blockj+3] = buf4;
                    B[i][blockj+4] = buf5;
                    B[i][blockj+5] = buf6;
                    B[i][blockj+6] = buf7;
                    B[i][blockj+7] = buf8;
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+i][blocki+j];
                        B[blockj+i][blocki+j] = B[blocki+j][blockj+i];
                        B[blocki+j][blockj+i] = buf1;
                    }
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+i][blocki+j+4];
                        B[blockj+i][blocki+j+4] = B[blocki+j][blockj+i+4];
                        B[blocki+j][blockj+i+4] = buf1;
                    }
                }
                
                for (i = blocki+4; i < blocki + 8; i++ ) {
                    buf1 = A[i][blockj];
                    buf2 = A[i][blockj+1];
                    buf3 = A[i][blockj+2];
                    buf4 = A[i][blockj+3];
                    buf5 = A[i][blockj+4];
                    buf6 = A[i][blockj+5];
                    buf7 = A[i][blockj+6];
                    buf8 = A[i][blockj+7];
                    
                    B[i][blockj+0] = buf1;
                    B[i][blockj+1] = buf2;
                    B[i][blockj+2] = buf3;
                    B[i][blockj+3] = buf4;
                    B[i][blockj+4] = buf5;
                    B[i][blockj+5] = buf6;
                    B[i][blockj+6] = buf7;
                    B[i][blockj+7] = buf8;
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+4+i][blocki+j];
                        B[blockj+4+i][blocki+j] = B[blocki+4+j][blockj+i];
                        B[blocki+4+j][blockj+i] = buf1;
                    }
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    for ( j = 0 ; j < i ; j++ ) {
                        buf1 = B[blockj+i+4][blocki+j+4];
                        B[blockj+i+4][blocki+j+4] = B[blockj+j+4][blocki+i+4];
                        B[blockj+j+4][blocki+i+4] = buf1;
                    }
                }
                
                for ( i = 0 ; i < 4 ; i++ ) {
                    
                    buf5 = B[blockj+i+4][blocki+0];
                    buf6 = B[blockj+i+4][blocki+1];
                    buf7 = B[blockj+i+4][blocki+2];
                    buf8 = B[blockj+i+4][blocki+3];
                    buf1 = B[blockj+i][blocki+4];
                    buf2 = B[blockj+i][blocki+5];
                    buf3 = B[blockj+i][blocki+6];
                    buf4 = B[blockj+i][blocki+7];
                    B[blockj+i+4][blocki+0] = buf1;
                    B[blockj+i+4][blocki+1] = buf2;
                    B[blockj+i+4][blocki+2] = buf3;
                    B[blockj+i+4][blocki+3] = buf4;
                    B[blockj+i][blocki+4] = buf5;
                    B[blockj+i][blocki+5] = buf6;
                    B[blockj+i][blocki+6] = buf7;
                    B[blockj+i][blocki+7] = buf8;
      
                }
            }
        }
    }
}

void transposeMN(int N, int M, int A[M][N], int B[N][M]) {
    
    int blockSizei = 16;
    int blockSizej = 4;
    int i, j, blocki, blockj;
    int buf1 = 0;
    
    for ( blocki = 0; blocki < M; blocki+= blockSizei ) {
        for ( blockj = 0; blockj < N; blockj+= blockSizej ) {
            for ( i = 0; i < (blockSizei) && i+blocki < M ; i++ ) {
                for ( j = 0; j < (blockSizej) && j+blockj < N; j++) {
                    if ( i == j ) {
                        buf1 = A[i+blocki][j+blockj];
                        continue;
                    }
                    B[j+blockj][i+blocki] = A[i+blocki][j+blockj];
                }
                for ( j = 0; j < (blockSizej) && j+blockj < N; j++) {
                    if ( i  == j ) {
                        B[j+blockj][i+blocki] = buf1;
                    }
                }
            }
        }
    }
}

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    
    if ( M == 32 && N == 32 ) {
        transpose3232(M, N, A, B);
    } else if ( M == 64 && N == 64) {
        transpose6464(M, N, A, B);
    } else {
        transposeMN(M, N, A, B);
    }
    
    ENSURES(is_transpose(M, N, A, B));
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
    
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
    
    ENSURES(is_transpose(M, N, A, B));
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

