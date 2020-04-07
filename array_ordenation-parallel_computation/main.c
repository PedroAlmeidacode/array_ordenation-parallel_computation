#include "library.h"



//fazer tempo de compilacao para melhorar o calculo de quantidade de filhos para cada quantidade de nmeros a organizar
// qual a quantidade de blocos a dividir um array para cada seqencia de modo a rentabilizar o tempo de execucao



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

    int n_blocks = N_CHILDS;    /*define numero de blocos a dividir array inicial, numeros de filhos a lancar para primeira operação*/
    int n_childs = number_of_process_created(n_blocks);
    int ints_per_block = n_sequencia / n_blocks;
    int ints_in_last_block = n_sequencia % ints_per_block;    /*contem o numero de inteiros no ultimo bloco do array*/

    //cobre o caso de haver um bloco apenas com 1 numero
    if (ints_in_last_block == 1) {
        ints_in_last_block = ints_per_block + 1; //junta o numero ao boco anterior
        n_blocks--; //retira o bloco anterior
    }

    printf("PARENT PROCESS : array with %d integers, total of %d childs will be launched", n_sequencia, n_childs);
    char **protocols = (char **) malloc(n_childs *
                                        sizeof(char *)); // guardara n_childs protocolos que sao as menagens que o pai vai receber de cada um dos filhos
    int *sequencia = new_int_array_substituition_exact_size(n_sequencia, sequencia_test,
                                                            1); //inicializa array seqeuncia com o seu tamnho exato

    printArray(sequencia, n_sequencia, "desordenada");/*imprime sequencia desordenada*/
    pid_t pids[n_childs]; /*variavel que vai conter o pid de todos os processos criados pelo processo pai para ordenar o array*/

    //instalacao do sinal
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);


    int i_protocol = 0;//itera as mensagens lidas no pai, enviadas do filho
    int last_pid = getpid();//variavel para casos exepcionais usada na leitura de protocolos pelo pai

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

        printf("PARENT PROCESS: launching %d childs\tints per block : %d\t ints in last block : %d\n", n_blocks,
               ints_per_block, ints_in_last_block);

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


            if (pids[n_pids] == 0) {
                /*___________________________________________________________________*
                *_______________________PROCESSO_FILHO_______________________________*
                * __________________________________________________________________*/


                if ((close(fd[0])) < 0) {
                    perror("CHILD PROCESS: closing reading from pipe descriptor in child process");// fechamos a leitura do pipe
                    exit(EXIT_FAILURE);
                }


                int *sub_sequencia = create_sub_array_from_array(sequencia, index_start, index_end,index_end - index_start + 1);
                //ordena sub-array
                mergesort_run(sub_sequencia, index_end - index_start + 1, 0, index_end - index_start);
                char *protocol = create_protocol(index_end, index_start, getpid(),sub_sequencia); // cria o protocolo e coloca o dentro do apontador
                freeIntArray(sub_sequencia);//liberta memoria nao necessaria


                printf("\nCHILD PROCESS [sub_sequencia %d]:\t pid : %d \t ppid: %d\t index_start : %d index_end : %d ints in block : %d\n Protocolo enviado [size:%lu]->",
                       i + 1, getpid(), getppid(), index_start, index_end, index_end - index_start + 1,
                       strlen(protocol));
                for (int j = 0; j < strlen(protocol); j++) {
                    printf("%c", *(protocol + j));
                }
                printf("\n");


                if (writen(fd[1], protocol, strlen(protocol)) == -1) {
                    perror("CHILD PROCESS: writting to pipe");
                    exit(EXIT_FAILURE);
                }


                free(protocol);//liberta mmeoria nao necessaria


                if ((close(fd[1])) < 0) {
                    perror("CHILD PROCESS: closing writting to pipe descriptor");//fechamos descritor de escrita do filho
                    exit(EXIT_FAILURE);
                }

                /* escrever subsequencia para ficheiro*/
                //fd[n_pids]=open("subsquencia.txt",O_WRONLY|O_CREAT|O_TRUNC,0644);
                //write(fd_sub_seq[n_pids],buf,index_end - index_start +1);


                kill(getppid(), SIGUSR1); // enviar sinal SIGUSR1
                exit(EXIT_SUCCESS);
            }
            i++; //itera block number por cada divisao da sequencia
            n_pids++; //itera numero de child lanacada
        }



        /*__________________________________________________________________
        *_______________________PROCESSO_PAI________________________________
        *__________________________________________________________________*/

        //fecho do seu descritor do lado de escrita do pipe.
        if ((close(fd[1])) < 0)perror("PARENT PROCESS: closing writting to pipe descriptor");


        char buf[4096];
        int start = 0, end = 0, childpid = 0, size_of_sub_sequencia = 0;
        int t = 0;
        //le protocolo do pipe dos filhos que terminarem
        while (readn(fd[0], buf, 4096) != 0) {
            t = 0;
            char *message = strtok(buf, "|"); // retira um protocolo de buff
            //retira protocolos para um array 2d
            while (message != NULL) {
                *(protocols + i_protocol) = (char *) malloc(strlen(message) * sizeof(char));
                strcpy(*(protocols + i_protocol), message);
                message = strtok(NULL, "|");
                t++;
                i_protocol++;
            }
            //descodifica cada um dos protocolos
            int max = i_protocol; //max indice in protocols pointer
            if (t > n_blocks) max = i_protocol - (t -n_blocks); // se tiver lido mais vezes do pipe do que devia a iteracao do loop sera feita n_block vezes
            for (int j = i_protocol - t; j < max; j++) {
                int *sub = get_data_from_protocol(&start, &end, &childpid, *(protocols + j),
                                                  &size_of_sub_sequencia);
                if (childpid > last_pid) { // para verificar que nao esta a receber nenhum processo da iteracao anterior
                    printf("PARENT PROCESS: Recebido protocolo-> index_start : %d, index_end: %d, child_pid: %d  ",
                           start, end, childpid);
                    printArray(sub, size_of_sub_sequencia, "");
                    int index_sub = 0;
                    for (int b = start; b <= end; b++) { // coloca sub sequencia ordenada na sequencia original
                        sequencia[b] = sub[index_sub];
                        index_sub++;

                    }
                    free(sub);
                }
            }
        }

        last_pid = childpid;


        if ((close(fd[0])) < 0)perror("PARENT PROCESS: closing reading from pipe descriptor in parent process");


        for (int j = 0; j < n_blocks; j++) {
            //esperar pelos filhos
            int status;
            if (waitpid(pids[j + start_point], &status, 0) == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            printf("PARENT PROCESS: filho numero: %d \t pid: %d terminou  \t ppid : %d \t status : %d\n",
                   j + start_point + 1, pids[j + start_point], getppid(), WEXITSTATUS(status));
        }
        start_point = n_pids; // atualiza o start point para que da proxima vez que esperar pelos filhos comece no id correto

        printf("PARENT PROCESS: child info::  sinais enviados - %d , protocolos lidos - %d , protocolos por ler - %d\n",
               childs_that_have_sent_protocol, t, n_blocks - t);



        /*se tiver 1 na variavel n_blocks neste ponto do codigo significa
        que o array esta organizado */
        if (n_blocks == 1) {
            free((protocols));
            printArray(sequencia, n_sequencia, "ordenada");
            /*escreve sequencia para o ficheiro*/
            /*if (writeInts(path, sequencia, n_sequencia) < 0) {
                perror("PARENT PROCESS: Reading file");
                exit(EXIT_FAILURE);
            }*/
            exit(EXIT_SUCCESS);
        }

        printArray(sequencia, n_sequencia, "\nordenada");




        /*divide o novo array por blocos sendo a nova
        divisao metade dos blocos anterior*/
        n_blocks = n_blocks / 2;
        /*atualiza numero de inteiros por bloco conforme o numero de blocos que divide a sequencia */
        ints_per_block = ints_per_block * 2;
        //ultimo bloco fica com o que sobra mais os inteiros normalmente atribuidos
        ints_in_last_block = n_sequencia - (ints_per_block * (n_blocks));
        printf("\n \n \n \n \n \n \n \n \n \n \n \n \n ");
    }

}


