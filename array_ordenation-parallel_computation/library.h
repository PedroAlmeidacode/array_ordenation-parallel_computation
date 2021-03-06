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
#include <fcntl.h>              /* Obtain O_* constant definitions */


#define N_CHILDS 10
#define N_MAX 9999
#define BUF_SIZE 4096
#define MAX_SIZE_SUB 400

long long time_usec_init;
long long time_usec_end1;
long long time_usec_end2;

int get_number_of_protocols(int ints_per_block, int rest_of_ints, int n_sequencia);
void mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int readInts(char * filename, int * intvec, int nmax);
int gettimeuseconds(long long * time_usec);
int uniform(int val_min, int val_max);
void printArray(int* a, int N,char string[]);
int * newIntArray(int N); // alloc new int array
void freeIntArray(int * v); // frees new int array
int uniformArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed values (returns -1 if error)
int uniformDistinctArray(int * a, int N, int val_min, int val_max); // function to fill an array with integer random uniformly distributed distinct values (returns -1 if error)
int binarySearch(int a[], int n, int key);
int writeInts(char * filename, int * intvec, int n);
int numberOfBlocksOfArray(int N,  int ints_per_block); //calculate number of blocks to inicaly divide the array
int number_of_process_created(int N); //calculates total of childs sent by the parent to ordenate the array
int get_index_end(int index_start, int ints_per_block, int n_blocks, int rest_of_ints, int n_sequencia, int i ); // calcula o index_end da subsequencia de acordo com varios parametros
void handler(); // handler que da catch a um SIGUSR1
ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, const void *ptr, size_t n);
char * create_protocol(int index_end,int index_start,int pid,int * sub_sequencia);
int * new_int_array_substituition_exact_size(int new_size, int * old_function, int to_free_put_one);/*depois de saber numero de inteiros a colocar no array da free e reneicializa com o seu tamanho exato*/
int * create_sub_array_from_array(int *array, int start, int end, int N);
int *get_data_from_protocol(int *sub_seq_index_start, int *sub_seq_index_end, int *childpid, char *buffer,int *size_of_subsequencia);
int * get_protocol_with_complement(int *a,char* complement_buffer, int size_of_original, int supposed_size);
int childs_that_have_sent_protocol;     // handler os sigusr1 increases this integer ,if the protocol has been readed the father does (-1)


#ifndef ARRAY_ORDENATION_PARALLEL_COMPUTATION_LIBRARY_H
#define ARRAY_ORDENATION_PARALLEL_COMPUTATION_LIBRARY_H

#endif //ARRAY_ORDENATION_PARALLEL_COMPUTATION_LIBRARY_H
