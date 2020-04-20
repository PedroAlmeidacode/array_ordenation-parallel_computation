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
    int rest_of_ints= n_sequencia % ints_per_block;    /*contem o numero de inteiros no ultimo bloco do array*/
    int n_protocols = get_number_of_protocols(ints_per_block,rest_of_ints,n_sequencia);



    printf("PARENT PROCESS : array with %d integers, total of %d childs will be launched", n_sequencia, n_childs);

    char **protocols = (char **) malloc(n_protocols * sizeof(char *)); // guardara n_childs protocolos que sao as menagens que o pai vai receber de cada um dos filhos

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

    /*Enquanto o array nao estiver organizado
 o segundo ciclo renova-se, ou seja o i volta a ser 0 e o numero de
 filhos/blocos é recalculado */
    while (1) {
        int i = 0, index_start , index_end = -1;
        //preserva o estado dos bounds pois casos a subsequencia seja subdividida para poder ser escrita para o pipe as variaveis bound vao ser alteradas
        childs_that_have_sent_protocol = 0;

        /* Descritor de ficheiros para as duas pontas do pipe*/
        int fd[2];
        /*Cria o pipe*/
        if (pipe(fd) == -1) {
            perror("pipe");
            exit(1);
        }

        printf("PARENT PROCESS: launching %d childs\tints per block : %d\t rest of ints: %d\n", n_blocks,
               ints_per_block, rest_of_ints);

        /*itera as vezes correspondentes ao numero de
        blocos que o array foi dividido*/
        while (i < n_blocks) {
            index_start = index_end + 1;
            index_end = get_index_end(index_start, ints_per_block, n_blocks, rest_of_ints, n_sequencia, i);

            //criar filhos
            if ((pids[n_pids] = fork()) == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }


            if (pids[n_pids] == 0) {
                /*___________________________________________________________________*
                *_______________________PROCESSO_FILHO_______________________________*
                * ___________________________________________________________________*/


                if ((close(fd[0])) < 0) {
                    perror("CHILD PROCESS: closing reading from pipe descriptor in child process");// fechamos a leitura do pipe
                    exit(EXIT_FAILURE);
                }


                int *sub_sequencia = create_sub_array_from_array(sequencia, index_start, index_end,
                                                                 index_end - index_start + 1);
                //ordena sub-array
                mergesort_run(sub_sequencia, index_end - index_start + 1, 0, index_end - index_start);


                /* escrever subsequencia para ficheiro*/
                char filename[n_pids >= 10 ? 6 : 5];
                sprintf(filename,"subsequencias/%d.txt",n_pids);
                open(filename,O_WRONLY|O_CREAT|O_TRUNC,0644);
                writeInts(filename, sub_sequencia, index_end-index_start+1);


                int index_start_sub = 0;
                int index_end_sub = index_end - index_start;
                int size_sub = index_end - index_start + 1;
                int end = index_end;
                int n_protocolos_for_sub_sequencia = 1;
                //se o tamamho da subsequencia for maior do que o suposto tera de se
                // dividir a sequencia, a ser enviada em varios protocolos
                if (index_end - index_start + 1 > MAX_SIZE_SUB) {
                    end= index_end_sub;
                    n_protocolos_for_sub_sequencia = (int) ((index_end - index_start + 1) / MAX_SIZE_SUB) + 1;
                    index_end_sub = MAX_SIZE_SUB - 1;
                    size_sub = MAX_SIZE_SUB;
                }


                int pid = getpid();
                //ciclo iterara se o numeros de protocolos a criar para a mesma subsequencia
                //for maior do que 1, no caso de o tamanho da subsequencia ultrapassar os limites do pipe
                for (int k = 0; k < n_protocolos_for_sub_sequencia ; k++) {


                    //no caso de ser a ultima iteracao o index_end sera igual ao index_end original
                    if (k == n_protocolos_for_sub_sequencia - 1) index_end_sub = end;

                    //passa o pedaco da subsequecia para um array
                    int *sub = create_sub_array_from_array(sub_sequencia, index_start_sub, index_end_sub,
                                                           size_sub);

                    char *protocol = create_protocol(index_start+index_end_sub, index_start+index_start_sub, pid,
                                                     sub); // cria o protocolo e coloca o dentro do apontador

                    freeIntArray(sub);
                    sub = NULL;

                    printf("\nCHILD PROCESS [sub_sequencia %d, parte %d]:\t pid : %d \t ppid: %d\t index_start : %d index_end : %d ints in block : %d\n Protocolo enviado [size:%lu size sub_sequencia:%d]->",
                           i + 1, k + 1, pid, getppid(),index_start+ index_start_sub, index_start+index_end_sub, size_sub ,
                           strlen(protocol), index_end_sub - index_start_sub + 1);
                    for (int j = 0; j < strlen(protocol); j++) {
                        printf("%c", *(protocol + j));
                    }
                    printf("\n");




                    if (writen(fd[1], protocol, strlen(protocol)) == -1) {
                        perror("CHILD PROCESS: writting to pipe");
                        exit(EXIT_FAILURE);
                    }

                    //free(protocol);//liberta mmeoria nao necessaria
                    //protocol = NULL;


                    index_start_sub = index_end_sub + 1;
                    index_end_sub = index_start_sub + MAX_SIZE_SUB - 1;

                }


                freeIntArray(sub_sequencia);//liberta memoria nao necessaria
                sub_sequencia=NULL;


                if ((close(fd[1])) < 0) {
                    perror("CHILD PROCESS: closing writting to pipe descriptor");//fechamos descritor de escrita do filho
                    exit(EXIT_FAILURE);
                }

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
        int start = 0, end = 0, childpid = 0, size_of_sub_sequencia = 0, t;
        int * temp  = NULL;
        int temp_size=0,supposed_size=0,temp_start = 0,temp_end = 0,temp_childpid = 0;

        //le protocolo do pipe dos filhos que terminarem
        while (readn(fd[0], buf, 4096) > 0) {
            t = 0;

            char *message = strtok(buf, "|"); // retira um protocolo de buff
            //retira protocolos para um array 2d
            while (message != NULL) {

                if(message[0] == '#'){
                    *(protocols + i_protocol) = (char *) malloc(strlen(message));
                    strcpy(*(protocols + i_protocol), message);
                    message = strtok(NULL, "|");
                    t++;
                    i_protocol++;
                    printf("protocolo lido\n");
                }else {
                    //se entrou neste ciclo quer dizer que tem uma parte de protocolo para juntar
                    if(temp_size != 0){ // quer dizer que guardou um protocolo que falta juntar uma porcao de protocolo

                        int *final = get_protocol_with_complement(temp,message,temp_size,supposed_size);

                        printf("final protcol supopposed size: %d , temp -size|%d| + message -size|%d|\n ",supposed_size,temp_size,supposed_size-temp_size);
                        printf("Protocol rceeived formed together childpid: %d\n",temp_childpid);
                        printArray(final, supposed_size, "Sub");

                        int index_sub = 0;
                        for (int b = temp_start; b <= temp_end; b++) { // coloca sub sequencia ordenada na sequencia original
                            sequencia[b] = final[index_sub];
                            index_sub++;
                        }

                        temp_start = 0,temp_end = 0,temp_childpid = 0,temp_size=0,supposed_size=0;
                        free(temp);
                        temp = NULL;
                        free(final);
                        final = NULL;
                    }
                    message = strtok(NULL, "|"); //em caso de lixo passado pelo pipe
                }
            }


            for (int j = i_protocol - t; j < i_protocol; j++) {


                int *sub = get_data_from_protocol(&start, &end, &childpid, *(protocols + j),&size_of_sub_sequencia);
                // se o protocolo extraido nao contiver o tamahho e conteudo suposto é necessario complementar com uma sequencia recebida nao enderessada por pid
                if(size_of_sub_sequencia< end-start+1){
                    //se o protocolo nao for os suposto ou nao estiver completo nao o colocamos no sitio
                    // podendo assim remediar o problema desde que esta nao seja a ultima divisao da seuquencia

                        temp_start = start,temp_end = end,temp_childpid = childpid,temp = sub,temp_size = size_of_sub_sequencia,supposed_size = end -start + 1;

                        printf("\033[0;34m");
                        printf("\n\nprotocolo [pid: %d] {%d-%d}; recebeu %d inteiros , perdeu %d inteiros\n", (int) childpid,start ,end,size_of_sub_sequencia,(end-start+1)-size_of_sub_sequencia );
                        printf("index_start : %d, index_end: %d, child_pid: %d ,size received = %d, size_sended : %d",
                           start, end, childpid, size_of_sub_sequencia, end - start + 1);
                        printArray(sub, size_of_sub_sequencia, "Sub");
                        printf("\033[0m\n");

                }
                else {// se o tamanho da sequencia for o esperado colocamos a sequencia no sitio
                    printf("\nPARENT PROCESS: Recebido protocolo-> index_start : %d, index_end: %d, child_pid: %d ,size_of_sub_sequencia = %d, size_sended : %d",
                           start, end, childpid, size_of_sub_sequencia, end - start + 1);
                    printArray(sub, size_of_sub_sequencia, "Sub");

                    int index_sub = 0;
                    for (int b = start; b <= end; b++) { // coloca sub sequencia ordenada na sequencia original
                        sequencia[b] = sub[index_sub];
                        index_sub++;
                    }
                    free(sub);
                }
            }
        }


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

        printf("PARENT PROCESS: child info::  sinais enviados : %d\n",
               childs_that_have_sent_protocol);


        /*se tiver 1 na variavel n_blocks neste ponto do codigo significa
        que o array esta organizado */
        if (n_blocks == 1) {
            if(protocols!=NULL)free((protocols));

            printArray(sequencia, n_sequencia, "ordenada");
            /*escreve sequencia para o ficheiro*/
            open("ordered.txt",O_WRONLY|O_CREAT|O_TRUNC,0644);
            if (writeInts("ordered.txt", sequencia, n_sequencia) < 0) {
                perror("PARENT PROCESS: Reading file");
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
        //ultimo bloco fica com o que sobra mais os inteiros normalmente atribuidos
        rest_of_ints = n_sequencia - (ints_per_block * (n_blocks));
        printf("\n \n \n \n \n \n \n \n \n \n \n \n \n ");
    }

}


