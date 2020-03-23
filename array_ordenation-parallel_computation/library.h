//
// Created by pedro on 23/03/20.
//

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#if defined _WIN64 || defined _WIN32


#else

#endif
#define BUFFSIZE 4096
#define N_MAX 200

void mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int readInts(char * filename, int * intvec, int nmax);
int gettimeuseconds(long long * time_usec);
int uniform(int val_min, int val_max);
void printArray(int* a, int N);
int * newIntArray(int N); // alloc new int array
void freeIntArray(int * v); // frees new int array
int uniformArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed values (returns -1 if error)
int uniformDistinctArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed distinct values (returns -1 if error)
int binarySearch(int a[], int n, int key);
int writeInts(char * filename, int * intvec, int n);
int numberOfBlocksToDivideArray(int N);


#ifndef ARRAY_ORDENATION_PARALLEL_COMPUTATION_LIBRARY_H
#define ARRAY_ORDENATION_PARALLEL_COMPUTATION_LIBRARY_H

#endif //ARRAY_ORDENATION_PARALLEL_COMPUTATION_LIBRARY_H
