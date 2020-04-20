//
// Created by pedro on 16/04/20.
//

#include "library.h"
#include "lib_util.c"

char *socket_path = "/tmp/socket";

//child vai receber em
// argv[1] -  path da sequencia a ler (string)
// argv[2] -  index start da subsequencia a ordenar (string)
// argv[3] -  index end da subsequencia a ordenar (string)
int main(int argc, char **argv) {
    int uds;
    struct sockaddr_un channel;        /* Unix Domain socket */

    if ((uds = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    memset(&channel, 0, sizeof(channel));
    channel.sun_family = AF_UNIX;
    strncpy(channel.sun_path, socket_path, sizeof(channel.sun_path) - 1);

    if (connect(uds, (struct sockaddr *) &channel, sizeof(channel)) == -1) {
        perror("connect error");
        exit(1);
    }
    //retira ranges de argv recebido
    int index_start = atoi(argv[2]);
    int index_end = atoi(argv[3]);

    int *sequencia_test = newIntArray(N_MAX);
    /*pai le inteiros contidos no ficheiro e armazena no array*/
    if (readInts(argv[1], sequencia_test, N_MAX) < 0) {
        perror("Reading file");
        exit(EXIT_FAILURE);
    }
    //cria subsequencia apartir de sequencia passando os ranges da sub pretendida
    int *sub_sequencia = create_sub_array_from_array(sequencia_test,index_start,index_end,index_end-index_start+1); //inicializa array sub seqeuncia com o seu tamnho exato

    //ordena sub-array
    mergesort_run(sub_sequencia, index_end - index_start + 1, 0, index_end - index_start);

    int pid = getpid();
    //cria protocolo
    char *protocol = create_protocol(index_end, index_start, pid,
                                     sub_sequencia); // cria o protocolo e coloca o dentro do apontador

    printf("\nCHILD.C [uds: %d pid: %d]:\tindex_start : %d index_end : %d ints in block : %d\n Protocolo enviado [size sub_sequencia:%d]->",
           uds, pid, index_start, index_end, index_end - index_start, index_end - index_start + 1);
    for (int j = 0; j < strlen(protocol); j++) {
        printf("%c", *(protocol + j));
    }
    printf("\n");

    //escrve protocolo para main scoket
    if (writen(uds, protocol, strlen(protocol)) == -1) {
        perror("CHILD PROCESS: writting to pipe");
        exit(EXIT_FAILURE);
    }

    free(sequencia_test);
    free(sub_sequencia);
    free(protocol);

    close(uds);
}