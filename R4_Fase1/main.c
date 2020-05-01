
#include "library.h"
#include "lib_util.c"


char *socket_path = "/tmp/socket";                                          // Unix domain socket name


int main(int argc, char *argv[]) {

    int *sequencia_test = newIntArray(
            N_MAX), n_sequencia, n_pids = 0;    /*inicializa dinamicamente sequencia de inteiros a ordenar com um tamanho definido por N_MAX*/


    /*pai le inteiros contidos no ficheiro e armazena no array*/
    if ((n_sequencia = readInts(argv[1], sequencia_test, N_MAX)) < 0) {
        perror("Reading file");
        exit(EXIT_FAILURE);
    }

    int n_blocks = N_CHILDS;    /*define numero de blocos a dividir array inicial, numeros de filhos a lancar para primeira operação*/
    int n_childs = number_of_process_created(n_blocks); // define o numero total de filhos a lancar dentro do while
    int ints_per_block = n_sequencia / n_blocks; // define o numero normal de inteiros por bloco
    int rest_of_ints = n_sequencia % ints_per_block;  //define o numero adicional de inteiros que o ultimo bloco de cada divisao tera de ler adicionalmente

    int listenfd, connfd, i;
    struct sockaddr_un channel_srv;

    //prepara a socket colocando uma listen queue de n_blocks ou seja no maximo podem estar na fila
    //de espera os blocos lancados na primeira iteracao
    listenfd = prepare_socket(channel_srv, socket_path, n_blocks);

    printf("PARENT PROCESS : array with %d integers, total of %d childs will be launched", n_sequencia, n_childs);

    //contera a sequencia original a ordenar (strlen exato)
    int *sequencia = new_int_array_substituition_exact_size(n_sequencia, sequencia_test,
                                                            1); //inicializa array seqeuncia com o seu tamnho exato

    printArray(sequencia, n_sequencia, "desordenada");/*imprime sequencia desordenada*/
    pid_t pids[n_childs]; /*variavel que vai conter o pid de todos os processos criados pelo processo pai para ordenar o array*/


    /*Enquanto o array nao estiver organizado
    o segundo ciclo renova-se, ou seja o i volta a ser 0 e o numero de
    filhos/blocos é recalculado */
    while (1) {
        i = 0;
        int index_start, index_end = -1;
        // variaveis passadas para filho pelo execv
        char index_start_char[5];
        char index_end_char[5];

        printf("PARENT PROCESS: launching %d childs\tints per block : %d\t rest of ints: %d\n", n_blocks,
               ints_per_block, rest_of_ints);

        /*itera as vezes correspondentes ao numero de
        blocos que o array foi dividido*/
        while (i < n_blocks) {
            index_start = index_end + 1;
            index_end = get_index_end(index_start, ints_per_block, n_blocks, rest_of_ints, n_sequencia, i);
            sprintf(index_start_char, "%d", index_start);
            sprintf(index_end_char, "%d", index_end);

            //criar filhos
            if ((pids[n_pids] = fork()) == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            if (pids[n_pids] == 0) {
                /*___________________________________________________________________*
                *_______________________PROCESSO_FILHO_______________________________*
                * ___________________________________________________________________*/
                char *args[5] = {"child.out", argv[1], index_start_char, index_end_char, NULL};

                execv("child.out", args);
                perror("execv");
                exit(EXIT_FAILURE);
            }

            i++; //itera block number por cada divisao da sequencia
            n_pids++; //itera numero de child lanacada
        }

        /*__________________________________________________________________
        *_______________________PROCESSO_PAI________________________________
        *__________________________________________________________________*/

        char buf[BUF_SIZE]; // armazenara o que receber da socket filho
        int start , end , childpid, size_of_sub_sequencia;

        for (int k = 0; k < n_blocks; k++) { // cicla por numero de blocos
            start = 0, end = 0, childpid = 0, size_of_sub_sequencia = 0;

            printf("\naceita filho numero %d\n",k+1);
            //accept new socket thta is in the listen queue
            if ((connfd = accept(listenfd, NULL, NULL)) == -1) {
                perror("accepts error;");
                continue;
            }

            //le protocolo dos sockets que enviarem
            while (readn(connfd, buf, BUF_SIZE) != 0) {

                char *message = strtok(buf, "|"); // retira um protocolo de buff
                char *protocol = (char *) malloc(strlen(message));// coloca o protocolo alocado
                strcpy(protocol, message);
                //retira informacao do protocolo para variaveis e sub sequencia para sub
                int *sub = get_data_from_protocol(&start, &end, &childpid, protocol,
                                                  &size_of_sub_sequencia);


                // se o tamanho da sequencia for o esperado colocamos a sequencia no sitio
                printf("\nPARENT PROCESS: Recebido protocolo-> index_start : %d, index_end: %d, child_pid: %d ,size_of_sub_sequencia = %d, size_sended : %d",
                       start, end, childpid, size_of_sub_sequencia, end - start + 1);
                printArray(sub, size_of_sub_sequencia, "Sub");

                int index_sub = 0;
                for (int b = start; b <= end; b++) { // coloca sub sequencia ordenada na sequencia original
                    sequencia[b] = sub[index_sub];
                    index_sub++;
                }
                free(sub);
                sub = NULL;
                protocol = NULL;
                message = NULL;
            }
        }
        /*se tiver 1 na variavel n_blocks neste ponto do codigo significa
        que o array esta organizado */
        if (n_blocks == 1) {
            printArray(sequencia, n_sequencia, "ordenada");
            exit(EXIT_SUCCESS);
        }
        printArray(sequencia, n_sequencia, "\nordenada");
        /*divide o novo array por blocos sendo a nova
        divisao metade dos blocos anterior*/
        n_blocks = n_blocks / 2;
        /*atualiza numero de inteiros por bloco conforme o numero de blocos que divide a sequencia */
        ints_per_block = ints_per_block * 2;
        //ultimo bloco fica com o que sobra mais os inteiros normalmente atribuidos
        rest_of_ints = n_sequencia - (ints_per_block * (n_blocks));
        printf("\n \n \n \n \n \n \n \n \n \n \n \n \n ");
    }
}

