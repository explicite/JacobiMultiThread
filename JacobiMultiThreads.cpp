/* 
 * File:   JacobiMultiThreads.cpp
 * Author: Jan Paw
 *
 * Created on 1 kwiecień 2012, 12:37
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <time.h>
#include <assert.h>


/*
 * 
 */

const int SIZE = 4;
int NUM_THRD = 4;

double  A[SIZE][SIZE],  b[SIZE], x1[SIZE], x2[SIZE], x3[SIZE];
double firstNorm;
double secondNorm;


void* count(void * slice);
void* initMatrix(void * slice);
float random(float a, float b);
bool errorCount();
void printX2();

int main(int argc, char** argv) {

    pthread_t threads[NUM_THRD];
    int thread_args[NUM_THRD];
    int rc, i;

    //Init Matrix
    for (i = 0; i < NUM_THRD; i++) {
        thread_args[i] = i;
        rc = pthread_create(&threads[i], NULL, initMatrix, (void *) &thread_args[i]);
        assert(0 == rc);
    }

    for (i = 0; i < NUM_THRD; i++) {
        rc = pthread_join(threads[i], NULL);
        assert(0 == rc);
    }

    do {
		
        for (int x = 0; x < SIZE; x++) {
            x3[x] = x1[x];
            x1[x] = x2[x];
        }

        //Count N * x + b
        for (i = 0; i < NUM_THRD; i++) {
            thread_args[i] = i;
            rc = pthread_create(&threads[i], NULL, count, (void *) &thread_args[i]);
            assert(0 == rc);
        }

        for (i = 0; i < NUM_THRD; i++) {
            rc = pthread_join(threads[i], NULL);
            assert(0 == rc);
        }



    } while (errorCount());

    printX2();


    return 0;
}

void printX2() {
    for (int i = 0; i < SIZE; i++)
        printf("%f\n", x2[i]);
}

bool errorCount() {
    double firstNorm = 0;
    double secondNorm = 0;

    for (int i = 0; i < SIZE; i++) {
        firstNorm += (x2[i] - x1[i])*(x2[i] - x1[i]);
        secondNorm += (x1[i] - x3[i])*(x1[i] - x3[i]);
    }


    if (sqrt(firstNorm) < sqrt(secondNorm))
        return false;
    else
        return true;
}

float random(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;

    return a + r;
}

void * initMatrix(void * slice) {
    int s;
    s = *((int*) slice);
    int rowStart = (s * SIZE) / NUM_THRD;
    int rowStop = ((s + 1) * SIZE) / NUM_THRD;

    srand(time(NULL));

    for (int i = rowStart; i < rowStop; i++) {
        /*for (int j = 0; j < SIZE; j++) {
            A[i][j] = 0;
            if (j == i) {
                A[i][j] = random(1, 100);
                b[i] = A[i][j];
            }
        }*/
        x1[i] = 0;
        x2[i] = 0;
        x3[i] = 0;
    }

	
    A[0][0] = 4;
    A[0][1] = -1;
    A[0][2] = -1;
    A[0][3] = 0;
    A[1][0] = -1;
    A[1][1] = 4;
    A[1][2] = 0;
    A[1][3] = -1;
    A[2][0] = -1;
    A[2][1] = 0;
    A[2][2] = 4;
    A[2][3] = -1;
    A[3][0] = 0;
    A[3][1] = -1;
    A[3][2] = -1;
    A[3][3] = 4;

    b[0] = 1;
    b[1] = 2;
    b[2] = 0;
    b[3] = 1;
	


    return NULL;
}

void * count(void * slice) {
    int s;
    s = *((int*) slice);
    int rowStart = (s * SIZE) / NUM_THRD;
    int rowStop = ((s + 1) * SIZE) / NUM_THRD;

    double sum;
    for (int i = rowStart; i < rowStop; i++) {
        sum = 0;
        for (int j = 0; j < SIZE; j++)
            if (j != i) sum += A[i][j] * x1[j];

        x2[i] = (b[i] - sum) / A[i][i];

    }

    return NULL;
}

