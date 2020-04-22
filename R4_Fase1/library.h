//
// Created by pedro on 16/04/20.
//

#ifndef R4_FASE1_LIBRARY_H
#define R4_FASE1_LIBRARY_H

#include <signal.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define BUF_SIZE 50000                                                      // block transfer size
#define N_CHILDS 10
#define N_MAX 20000

int prepare_socket(struct sockaddr_un channel_srv, char *  socket_path, int listenq);


void mergesort_run(int *a, int n , int lo, int hi);
void merge_array(int *a, int *aux, int lo, int mid, int hi);
void mergesort_recursivo(int *a, int *aux, int lo, int hi);
int readInts(char * filename, int * intvec, int nmax);
int gettimeuseconds(long long * time_usec);
void printArray(int* a, int N,char string[]);
int * newIntArray(int N); // alloc new int array
void freeIntArray(int * v); // frees new int array
int number_of_process_created(int N); //calculates total of childs sent by the parent to ordenate the array
int get_index_end(int index_start, int ints_per_block, int n_blocks, int rest_of_ints, int n_sequencia, int i ); // calcula o index_end da subsequencia de acordo com varios parametros
ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, const void *ptr, size_t n);
char * create_protocol(int index_end,int index_start,int pid,int * sub_sequencia);
int * new_int_array_substituition_exact_size(int new_size, int * old_function, int to_free_put_one);/*depois de saber numero de inteiros a colocar no array da free e reneicializa com o seu tamanho exato*/
int *get_data_from_protocol(int *sub_seq_index_start, int *sub_seq_index_end, int *childpid, char *buffer,int *size_of_subsequencia);



#endif //R4_FASE1_LIBRARY_H
