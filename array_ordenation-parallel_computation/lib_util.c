#include "library.h"

int get_index_end(int index_start, int ints_per_block, int n_blocks, int ints_in_last_block, int n_sequencia, int i) {
    //se estiver no ultimo filho que reorganiza todos o array
    if (n_blocks == 1) {
        return n_sequencia - 1;
    }
    //se estiver no ultimo bloco e nao na ultima child lancada
    if (i + 1 == n_blocks && ints_in_last_block != 0) {

        return index_start + (ints_in_last_block - 1);
    }
    return index_start + (ints_per_block - 1);
}

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

int numberOfBlocksOfArray(int N, int ints_per_block) {
    //se o numeros nao chegar ao numero minimo de inteiros por bloco apenas tera um bloco
    if (N == ints_per_block)return 1;
    // se a divisao nao der resto quer dizer que é uma divisao perfeita e todos os blocos terao o mesmo numero de inteiros
    if ((N % ints_per_block) == 0)return (int) N / ints_per_block;
    // se der resto diferente de 0 quer dizer que tem de ser feito mais um bloco com um numero de inteiros diferente dos outros blocos
    return ((int) (N / ints_per_block)) + 1;
}


int number_of_process_created(int N) {
    int n_blocks = N;
    while (N != 1) {
        N = N / 2;
        n_blocks = n_blocks + N;
    }
    return n_blocks;
}

void printArray(int *a, int N, char string[]) {
    int i;
    printf("Sequencia %s:\n", string);
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
        *(aux + k) = *(a + k);
    int i = lo, j = mid + 1;
    for (k = lo; k <= hi; k++) {
        if (i > mid) *(a + k) = *(aux + (j++));
        else if (j > hi) *(a + k) = *(aux + (i++));
        else if (*(aux + j) < *(aux + i)) *(a + k) = *(aux + (j++));
        else *(a + k) = *(aux + (i++));
    }
}

void mergesort_recursivo(int *a, int *aux, int lo, int hi) {
    if (hi <= lo) return;
    int mid = lo + (hi - lo) / 2;
    mergesort_recursivo(a, aux, lo, mid);
    mergesort_recursivo(a, aux, mid + 1, hi);
    if (*(a + (mid + 1)) >= *(a + mid)) return; // improvement
    merge_array(a, aux, lo, mid, hi);
}

void mergesort_run(int *a, int n, int lo, int hi) {
    int *aux;
    aux = malloc(sizeof(int) * n);
    mergesort_recursivo(a, aux, lo, hi);
    free(aux);
}

void handler() {
    printf("subsequencia ordenada");
}


int get_ints_per_block(int n_sequencia) {

    int min = MIN_INTS_PER_BLOCK;
    int max = MAX_INTS_PER_BLOCK;
    if (n_sequencia > 100) { //se o array for muito grande evito que faca sublocos de 2 inteiros
        min = 10;
        max = 20;
    }
    int predefined = max;
    while (predefined != min) {
        if (n_sequencia % predefined == 0)
            return predefined;
        predefined--;
    }
    //se o numero de inteiros no array fr menor ou igual ao numero estipulado o bloco tera o numero de inteiros do array
    if (n_sequencia <= MAX_INTS_PER_BLOCK) {
        return n_sequencia;
    }
    return max; // signifa que nao tem um dividor perfeito retorna o numero estipulado
}


ssize_t readn(int fd, void *ptr, size_t n)
{
    size_t      nleft;
    ssize_t     nread;
    nleft = n;
        while (nleft > 0) {
            if (( nread = read(fd, ptr, nleft)) < 0) {
                if (nleft == n)
                    return(-1); /* error, return -1 */
                else
                    break;
            } else if (nread == 0) {
                break;
            }
            nleft -= nread;
            ptr += nread;
        }
        return(n - nleft);
}


ssize_t writen(int fd, const void *ptr, size_t n)
{
    size_t nleft;
    ssize_t nwritten;

    nleft = n;
    while (nleft > 0) {
        if (( nwritten = write(fd, ptr, nleft) ) < 0) {
            if (nleft == n)
                return(-1); /* error, return -1 */
            else
                break;
        } else if (nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return(n - nleft);
}

char * create_protocol(int index_end,int index_start,int pid, int * sub_sequencia, int * size_protocol){
    int size_sub_array = index_end - index_start +1 ; //tamanho array
    int size_aux = size_sub_array + (size_sub_array-1);
    char aux[BUF_SIZE];
    int k =0, s =0;
    while(k < size_sub_array) {
        int j =0;
        if((sub_sequencia[k] / 10) > 0 ){//inteiros de dois digitos
            size_aux++, j++;
        }
        if((sub_sequencia[k] / 100) > 0 ){ //inteiros de 3 digitos
            size_aux+=2, j+=2;
        }

        sprintf(&aux[k+s], "%d", sub_sequencia[k]);
        if(k+s+j+1 != size_aux) aux[k+s+j+1] = ',';
        s++,k++;
        if(j!=0)s++;
    }
    char auxx[size_aux];
    strcpy(auxx,aux);

    char protocol[BUF_SIZE];
    sprintf(protocol,"#%d*%d;%d*%s|",pid,index_start,index_end,auxx);
    int size = 0;
    while(protocol[size] != '|')size++;
    *size_protocol = size+1;
    char *a = (char *) malloc(size + 1); // incluindo o '|'
    strcpy(a,protocol);
    return a;
}



int * get_data_from_protocol(int * sub_seq_index_start, int * sub_seq_index_end, int *childpid, char * buffer, int * size_of_subsequencia) {

    char delimiter[] = "#*;";
    char *message = strtok(buffer, delimiter);
    int i = 0;
    while (i < 4) {
        if (i == 0)*childpid = atoi(message);
        if (i == 1)*sub_seq_index_start = atoi(message);
        if (i == 2) *sub_seq_index_end = atoi(message);
        if (i == 3) {
            int n = 0;
            char *sub = strtok(message, ",");
            int a[BUF_SIZE];
            while (n >= 0) {
                if (sub == NULL) {
                    *size_of_subsequencia = n;
                    int *sub_sequencia = newIntArray(n);
                    for (int k = 0; k < n; k++) {
                        sub_sequencia[k] = a[k];
                    }
                    return sub_sequencia;
                }
                a[n] = atoi(sub);
                n++;
                sub = strtok(NULL, ",");
            }
        }
        message = strtok(NULL, delimiter);
        i++;
    }
}


    int * new_int_array_substituition_exact_size(int new_size, int * old_function, int to_free_put_one){
    int *a = newIntArray(new_size);
    for (int k = 0; k < new_size ; k++) {
        *(a+k) = *(old_function+k);
    }
    if(to_free_put_one == 1)  freeIntArray(old_function);
    return a;
}

int * create_sub_array_from_array(const int * array, int start, int end, int new_size){
    int k =0;
    int *sub = newIntArray(new_size);
    for (int j = start; j <= end ; j++) {
        *(sub+k)=*(array+j);
        k++;
    }
    return sub;
}



#endif
