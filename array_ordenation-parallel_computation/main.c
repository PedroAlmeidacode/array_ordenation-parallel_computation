#include "library.h"

/* perguntas
* como devo calcular o numero de filhos a lancar ?
* como funciona o readn quando enche a pipe
* quando divido 3 blocos por 2 resulta 1 , devo arredondar para 2 ? para ser mais rentavel ?
 * nao envia o numero de sinais orrespondentes ao numero de filhos que finalizaram
 * nao le todos os filhos no readn e estraga o resto dos ciclos
 * nao volta a ler nada do pipe no readn nos ciclos restantes
 *resolver problema nao poder continua a dividir array enquanto nao ler e colocar no sitio todos os protocolos da primeira divisap
 *
*/




int main(int argc, char *argv[]) {
    int *sequencia_test = newIntArray(
            N_MAX), n_sequencia, n_pids = 0, start_point = 0;    /*inicializa dinamicamente sequencia de inteiros a ordenar com um tamanho definido por N_MAX*/
    /*processo pai le path de *argv ,input do terminal*/
    char path[100];
    printf("\nIntroduza o camimho do ficheiro:\n");
    scanf("%99[^\n]%*c", path);

    /*pai le inteiros contidos no ficheiro e armazena no array*/
    if ((n_sequencia = readInts(path, sequencia_test, N_MAX)) < 0) {
        perror("Reading file");
        exit(EXIT_FAILURE);
    }

    //int ints_per_block = get_ints_per_block(n_sequencia);
    int ints_per_block = 10;
    int n_blocks = numberOfBlocksOfArray(n_sequencia,ints_per_block);    /*define numero de blocos a dividir array inicial, numeros de filhos a lancar para primeira operação*/

    int ints_in_last_block = n_sequencia % ints_per_block;    /*contem o numero de inteiros no ultimo bloco do array*/

    //cobre o caso de haver um bloco apenas com 1 numero
    if (ints_in_last_block == 1) {
        ints_in_last_block = ints_per_block + 1; //junta o numero ao boco anterior
        n_blocks--; //retira o bloco anterior
    }

    int n_childs = number_of_process_created(n_blocks);
    char **protocols = (char **) malloc(n_childs *
                                        sizeof(char *)); // guardara n_childs protocolos que sao as menagens que o pai vai receber de cada um dos filhos
    int *sequencia = new_int_array_substituition_exact_size(n_sequencia, sequencia_test,
                                                            1); //inicializa array seqeuncia com o seu tamnho exato

    printArray(sequencia, n_sequencia, "desordenada");/*imprime sequencia desordenada*/
    //int fd_sub_seq[n_childs];
    pid_t pids[n_childs]; /*variavel que vai conter o pid de todos os processos criados pelo processo pai para ordenar o array*/

    //instalacao do sinal
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);


    int i_child = 0;//itera as mensagens lidas no pai, enviadas do filho

    /*Enquanto o array nao estiver organizado
 o segundo ciclo renova-se, ou seja o i volta a ser 0 e o numero de
 filhos/blocos é recalculado */
    while (1) {
        int i = 0, index_start = 0, index_end = -1;
        childs_that_have_sent_protocol = 0;

        /* Descritor de ficheiros para as duas pontas do pipe*/
        int fd[2];

        /*Cria o pipe*/
        if (pipe(fd) == -1) {
            perror("pipe");
            exit(1);
        }

        /*itera as vezes correspondentes ao numero de
        blocos que o array foi dividido*/
        while (i < n_blocks) {
            index_start = index_end + 1;
            index_end = get_index_end(index_start, ints_per_block, n_blocks, ints_in_last_block, n_sequencia, i);

            //criar filhos
            if ((pids[n_pids] = fork()) == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }

            /*PROCESSO FILHO*/
            if (pids[n_pids] == 0) {

                if ((close(fd[0])) < 0)
                    perror("closing reading from pipe descriptor in child process");// fechamos a leitura do pipe
                int *sub_sequencia = create_sub_array_from_array(sequencia, index_start, index_end,
                                                                 index_end - index_start + 1);

                //ordena sub-array
                mergesort_run(sub_sequencia, index_end - index_start + 1, 0, index_end - index_start);

                int size_protcol = 0;
                char *protocol = create_protocol(index_end, index_start, getpid(), sub_sequencia, &size_protcol);
                freeIntArray(sub_sequencia);//liberta memoria nao necessaria
                printf("Protocolo enviado [size:%d]->", size_protcol);
                for (int j = 0; j < size_protcol; j++) {
                    printf("%c", *(protocol + j));
                }
                printf("\n");
                if (writen(fd[1], protocol, size_protcol) == -1) perror("writting to pipe");
                free(protocol);//liberta mmeoria nao necessaria


                if ((close(fd[1])) < 0)
                    perror("closing writting to pipe descriptor in child process");//fechamos descritor de escrita do filho

                /* escrever subsequencia para ficheiro*/
                //fd[n_pids]=open("subsquencia.txt",O_WRONLY|O_CREAT|O_TRUNC,0644);
                //write(fd_sub_seq[n_pids],buf,index_end - index_start +1);


                kill(getppid(), SIGUSR1); // enviar sinal SIGUSR1

                exit(EXIT_SUCCESS);
            }
            i++; //itera block number por cada divisao da sequencia
            n_pids++; //itera numero de child lanacada
        }


        /*PROCESSO PAI*/

        //fecho do seu descritor do lado de escrita do pipe.
        if ((close(fd[1])) < 0)perror("closing writting to pipe descriptor in parent process");

        char buf[4096];
        int start = 0, end = 0, childpid = 0, size_of_sub_sequencia = 0;

        int t = 0;

        //le protocolo do pipe dos filhos que terminarem

        while (readn(fd[0], buf, 4096) != 0) {
            t = 0;
            char *message = strtok(buf, "|"); // retira um protocolo de buff

            //retira protocolos para um array 2d
            while (message != NULL) {
                *(protocols + i_child) = (char *) malloc(strlen(message) * sizeof(char));
                strcpy(*(protocols + i_child), message);
                message = strtok(NULL, "|");
                t++;
                i_child++;
            }

            //descodifica cada um dos protocolos
            for (int j = i_child - t; j < i_child; j++) {
                int *sub = get_data_from_protocol(&start, &end, &childpid, *(protocols + j),
                                                  &size_of_sub_sequencia);
                printf("Recebido protocolo-> index_start : %d, index_end: %d, child_pid: %d  ", start, end,
                       childpid);
                printArray(sub, size_of_sub_sequencia, "");
                int index_sub = 0;
                for (int b = start; b <= end; b++) { // coloca sub sequencia ordenada na sequencia original
                    sequencia[b] = sub[index_sub];
                    index_sub++;
                }
                free(sub);
            }
        }


        if ((close(fd[0])) < 0)perror("closing reading from pipe descriptor in parent process");


        for (int j = 0; j < n_blocks; j++) {
            //esperar pelos filhos
            if (waitpid(pids[j + start_point], NULL, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            printf("filho numero: %d \t pid: %d terminou \n", j + start_point + 1, pids[j + start_point]);
        }
        start_point = n_pids; // atualiza o start point para que da proxima vez que esperar pelos filhos comece no id correto


        printf("child info::  sinais enviados - %d , protocolos lidos - %d , protocolos por ler - %d\n",
               childs_that_have_sent_protocol, t, n_blocks - t);


        /*se tiver 1 na variavel n_blocks neste ponto do codigo significa
        que o array esta organizado */
        if (n_blocks == 1) {
            free((protocols));
            printArray(sequencia, n_sequencia, "ordenada");
            /*escreve sequencia para o ficheiro*/
            if (writeInts(path, sequencia, n_sequencia) < 0) {
                perror("Reading file");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
        }

        printArray(sequencia, n_sequencia, "\nordenada");

        /*divide o novo array por blocos sendo a nova
        divisao metade dos blocos anterior*/
        n_blocks = n_blocks / 2;
        /*atualiza numero de inteiros por bloco conforme o numero de blocos que divide a sequencia */
        ints_per_block = ints_per_block * 2;
        //ultimo bloco fica com o que sobra
        ints_in_last_block = n_sequencia - (ints_per_block * (n_blocks - 1));
        printf("\n \n \n \n \n \n \n \n \n \n \n \n \n ");
    }

}


