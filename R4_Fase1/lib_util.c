#include "library.h"


int get_index_end(int index_start, int ints_per_block, int n_blocks, int rest_of_ints, int n_sequencia, int i) {
    //se estiver no ultimo filho que reorganiza todos o array
    if (n_blocks == 1) return n_sequencia - 1;
    //se estiver no ultimo bloco e nao na ultima child lancada
    if (i + 1 == n_blocks) return index_start + ints_per_block + (rest_of_ints - 1);

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
    printf("\n %s Sequencia:\n", string);
    for (i = 0; i < N; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
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

ssize_t readn(int fd, void *ptr, size_t n) {
    size_t nleft;
    ssize_t nread;
    nleft = n;
    while (nleft > 0) {
        if ((nread = read(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return (-1); /* error, return -1 */
            else
                break;
        } else if (nread == 0) {
            break;
        }
        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);
}


ssize_t writen(int fd, const void *ptr, size_t n) {
    size_t nleft;
    ssize_t nwritten;

    nleft = n;
    while (nleft > 0) {
        if ((nwritten = write(fd, ptr, nleft)) < 0) {
            if (nleft == n)
                return (-1); /* error, return -1 */
            else
                break;
        } else if (nwritten == 0) {
            break;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return (n - nleft);
}



char *create_protocol(int index_end, int index_start, int pid, int *sub_sequencia) {
    int size_sub_array = index_end - index_start + 1; //tamanho array
    int size_aux = size_sub_array + (size_sub_array - 1);
    char aux[BUF_SIZE];
    memset(aux, 0, sizeof(aux));
    int k = 0, s = 0, i = 0;
    while (k < size_sub_array) {
        int j = 0;
        if ((sub_sequencia[k] / 100) > 0) { //inteiros de 3 digitos
            size_aux += 2, j += 2, i++;
        } else if ((sub_sequencia[k] / 10) > 0) {//inteiros de dois digitos
            size_aux++, j++;
        }

        sprintf(&aux[k + s], "%d", sub_sequencia[k]);
        if (k + s + j + 1 != size_aux) aux[k + s + j + 1] = ',';

        s++, k++;
        if (j != 0)s++;
        if (i == 1)s++;
        i = 0;
    }
    char auxx[size_aux];
    memset(auxx,0,sizeof(auxx));
    strcpy(auxx, aux);

    char protocol[BUF_SIZE];
    memset(protocol,0,sizeof(protocol));
    sprintf(protocol, "#%d*%d;%d*%s|", pid, index_start, index_end, auxx);
    int size = 0;
    while (protocol[size] != '|')size++;
    char *a = (char *) malloc(size + 1); // incluindo o '|'
    strcpy(a, protocol);
    return a;
}

/**
 * Esta funcao retorna um aontador para a sequencia recebida do filho
 * se a sequencia nao tiver todos os elemntos a funcao retorna uma apontador para um protovolo com o tamanho suposto mas
 * passa como tamamnho size_of_subsequencia o tamanho lido do protocolo (nao o suposto)
 * @param sub_seq_index_start start suposto
 * @param sub_seq_index_end end suposto
 * @param childpid
 * @param buffer
 * @param size_of_subsequencia - tamanho real da sequencia de inteiros obtida
 * @param complement_buffer
 * @param used
 * @return
 */
int *get_data_from_protocol(int *sub_seq_index_start, int *sub_seq_index_end, int *childpid, char *buffer,int *size_of_subsequencia) {
    char delimiter[] = "#*;|";
    char *message = strtok(buffer, delimiter);
    int end=0, start =0;

    int i = 0;
    while (i < 4) {

        if (message != NULL) {
            if (i == 0)*childpid = atoi(message);
            if (i == 1){
                start = atoi(message);
                *sub_seq_index_start = start;
            }
            if (i == 2) {
                end = atoi(message);
                *sub_seq_index_end = end;
            }


            if (i == 3) {
                int size_protocol = end - start + 1;

                int n = 0;

                char *sub = strtok(message, ",|");
                int a[size_protocol];

                while (n >= 0)
                {
                    if (sub == NULL)
                    {

                        *size_of_subsequencia = n;
                        int *sub_sequencia = newIntArray(n);//alloca com o tamanho real da sequencia e nao o que deveria ter

                        //iteracao para protocolos que foram lidos completos da pipe
                        for (int k = 0; k < n; k++) {
                            sub_sequencia[k] = a[k];
                        }
                        return sub_sequencia;
                    }
                    a[n] = atoi(sub);
                    n++;
                    sub = strtok(NULL, ",|");
                }
            }
            message = strtok(NULL, delimiter);
            i++;
        }
    }
    return NULL;
}


int prepare_socket(struct sockaddr_un channel_srv, char *socket_path, int listenq) {

    int listenfd;

    if ((listenfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {              // Creating the server socket
        perror("socket error");
        exit(-1);
    }

    unlink(socket_path);

    memset(&channel_srv, 0, sizeof(channel_srv));
    channel_srv.sun_family = AF_UNIX;
    strncpy(channel_srv.sun_path, socket_path, sizeof(channel_srv.sun_path) - 1);

    if (bind(listenfd, (struct sockaddr *) &channel_srv, sizeof(channel_srv)) ==
        -1) {      // Binding the server socket to its name
        perror("bind error");
        exit(-1);
    }
    if (listen(listenfd, listenq) == -1) {                                  // Configuring the listen queue
        perror("listen error");
        exit(-1);
    }
    //ate aqui a instalação
    return listenfd;
}

int *new_int_array_substituition_exact_size(int new_size, int *old_function, int to_free_put_one) {
    int *a = newIntArray(new_size);
    for (int k = 0; k < new_size; k++) {
        *(a + k) = *(old_function + k);
    }
    if (to_free_put_one == 1) freeIntArray(old_function);
    return a;
}

int * create_sub_array_from_array(int *array, int start, int end, int N) {
    int k = 0;
    int * a = newIntArray(N);
    for (int j = start; j <= end; j++) {
        *(a + k) = *(array + j);
        k++;
    }
    return a;
}



#endif
