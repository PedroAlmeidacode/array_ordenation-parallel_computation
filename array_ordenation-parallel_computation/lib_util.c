#include "library.h"

int readInts(char *filename, int *intvec, int nmax) {
    FILE *fp;
    int n = 0;
    fp = fopen(filename, "r");
    if (fp != NULL) {
        while (!feof(fp) && n < nmax)
            fscanf(fp, "%d\n", &intvec[n++]);
        fclose(fp);
    } else return -1; // erro na leitura do ficheiro
    return n;
}

#ifdef WIN32
int gettimeuseconds(long long * time_usec) {
    union {
        long long ns100; //time since 1 Jan 1601 in 100ns units
        FILETIME ft;
    } now;
    
    GetSystemTimeAsFileTime( &(now.ft) ); // win32 function!
    *time_usec=(long long) (now.ns100 / 10LL);
    return 0;
}
#else

int gettimeuseconds(long long *time_usec) {
    struct timeval time;
    gettimeofday(&time, NULL);

    *time_usec = (long long) (time.tv_sec * 1000000 + time.tv_usec);
    return 0;
}



/** 
  @brief gera um inteiro aleatoriamente/uniformemente no intervalo: val_min...val_max 
 */
int uniform(int val_min, int val_max) {
    return val_min + rand() % (val_max - val_min + 1);
}

/// @brief function to fill an array with integer random uniformly distributed values (returns -1 if error)
int uniformArray(int *a, int N, int val_min, int val_max) {
    int i;
    for (i = 0; i < N; i++) {
        a[i] = uniform(val_min, val_max);
    }
    return 0;
}

/**
 *  @brief int array dynamic managing
 *
 *  @param N size of int array to be created
 *
 *  @return new int array allocate in heap memory
 */
int *newIntArray(int N) {
    return (int *) malloc(sizeof(int) * N);
}

void freeIntArray(int *v) {
    free(v);
}

int numberOfBlocksToDivideArray(int N){
    //se o numeros nao chegar ao numero minimo de inteiros por bloco apenas tera um bloco
    if(N <= INTS_PER_BLOCK)return 1;
    // se a divisao nao der resto quer dizer que é uma divisao perfeita e todos os blocos terao o mesmo numero de inteiros
    if((N % INTS_PER_BLOCK) == 0)return (int) N / INTS_PER_BLOCK;
    // se der resto diferente de 0 quer dizer que tem de ser feito mais um bloco com um numero de inteiros diferente dos outros blocos
    return ((int) (N / INTS_PER_BLOCK)) + 1;
}


int number_of_process_created(int N){
    int a = N;
    while(N != 1){
        N = N / 2;
        a = a + N;
    }
    return  a;
}

void printArray(int *a, int N) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}

int binarySearch(int a[], int n, int key) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (key < a[mid]) hi = mid - 1;
        else if (key > a[mid]) lo = mid + 1;
        else return mid;
    }
    return -1;
}

int writeInts(char *filename, int *intvec, int n) {
    FILE *fp;
    int i = 0;
    fp = fopen(filename, "w");
    if (fp != NULL) {
        while (i < n)
            fprintf(fp, "%d\n", intvec[i++]);
        fclose(fp);
    } else return -1; // erro na leitura do ficheiro
    return 0;
}

void merge_array(int *a, int *aux, int lo, int mid, int hi) {
    int k;
    for (k = lo; k <= hi; k++)
        *(aux+k) = *(a+k);
    int i = lo, j = mid+1;
    for (k = lo; k <= hi; k++) {
        if (i > mid) *(a+k) = *(aux+(j++));
        else if (j > hi) *(a+k) = *(aux+(i++));
        else if (*(aux+j) < *(aux+i)) *(a+k) = *(aux+(j++));
        else *(a+k) = *(aux+(i++));
    }
}

void mergesort_recursivo(int *a, int *aux, int lo, int hi) {
    if (hi <= lo) return;
    int mid = lo + (hi - lo) / 2;
    mergesort_recursivo(a, aux, lo, mid);
    mergesort_recursivo(a, aux, mid+1, hi);
    if (*(a+(mid+1)) >= *(a+mid)) return; // improvement
    merge_array(a, aux, lo, mid, hi);
}

void mergesort_run(int *a, int n , int lo, int hi) {
    int * aux;
    aux = malloc(sizeof(int) * n);
    mergesort_recursivo(a, aux, lo, hi);
    free(aux);
}
#endif