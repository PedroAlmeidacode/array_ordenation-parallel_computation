#include "library.h"


int main(int argc, char *argv[]) {

    /*inicializa dinamicamente sequencia de inteiros a ordenar com um tamanho definido por N_MAX*/
    int *sequencia_test = newIntArray(N_MAX);
    int n_sequencia;



    /*processo pai le path de *argv ,input do terminal*/
    char path[100];
    printf("\nIntroduza o camimho do ficheiro:\n");
    scanf("%99[^\n]%*c", path);



    /*pai le inteiros contidos no ficheiro e armazena no array*/
    if ((n_sequencia = readInts(path, sequencia_test, N_MAX)) < 0) {
        perror("Reading file");
        exit(0);
    }




    /*depois de saber numero de inteiros a colocar no array da free e reneicializa com o seu tamanho exato*/
    int *sequencia = newIntArray(n_sequencia);
    for (int k = 0; k < n_sequencia ; k++) {
        *(sequencia+k) = *(sequencia_test+k);
    }
    freeIntArray(sequencia_test);



    /*imprime sequencia desordenada*/
    printf("Sequencia desordenada:\n");
    printArray(sequencia, n_sequencia);



    /*define numero de blocos a dividir array inicial, numeros de filhos a lancar para primeira operação*/
    int n_blocks = numberOfBlocksToDivideArray(n_sequencia);
    int n_childs = number_of_process_created(n_blocks);
    int ints_in_last_block = n_sequencia % INTS_PER_BLOCK;



    /* Descritor de ficheiros para as duas pontas do pipe*/
    int fd[2];



    int pids[n_childs]; //variavel que vai conter o pid de todos os processos criados pelo processo pai para ordenar o array




    /*Cria o pipe*/
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(1);
    }




    int n_pids = 0, start_point =0, ints_per_block= INTS_PER_BLOCK;
    if(n_sequencia <= INTS_PER_BLOCK) {
        ints_per_block = n_sequencia;
    }



    /*Enquanto o array nao estiver organizado
    o segundo ciclo renova-se, ou seja o i volta a ser 0 e o numero de
    filhos/blocos é recalculado */
    while (1) {


        int i = 0, index_start = 0, index_end =-1;




        /*itera as vezes correspondentes ao numero de
        blocos que o array foi dividido*/
        while (i < n_blocks) {
            index_start = index_end + 1 ;
            index_end = index_start + (ints_per_block - 1) ;

            //se estiver no ultimo bloco e nao na ultima child lancada
            if(i+1 == n_blocks && n_pids+1){
                index_end = index_start + (ints_in_last_block - 1);
            }
            //se estiver no ultimo filho que reorganiza todos o array
            if(n_blocks == 1){
                index_end = n_sequencia - 1;
            }




            //criar filhos
            if ((pids[n_pids] = fork()) == -1) {
                perror("fork");
                exit(-1);
            }


            /*PROCESSO FILHO*/
            if (pids[n_pids] == 0) {
                printf("execucao filho numero: %d \t pid: %d no intervalo de sequencia [%d,%d]\n",n_pids+1, getpid(), index_start, index_end);
                mergesort_run(sequencia, n_sequencia, index_start, index_end);
                exit(EXIT_SUCCESS);
            }



            i++; //itera block number por cada divisao da sequencia
            n_pids++; //itera numero de child lanacada
        }

        /*PROCESSO PAI*/


        for (int j = 0; j < n_blocks; j++) {
            //esperar pelos filhos
            waitpid(pids[j+start_point], NULL, 0);
            printf("filho numero: %d \t pid: %d terminou \n",j+start_point+1,pids[j+start_point]);
        }
        start_point = n_pids; // atuaiza o start points para que da proxima vez que esperar pelos filhos comece no id correto





        /*se tiver 1 na variavel n_blcks neste ponto do codigo significa
    que o array esta organizado */
        if (n_blocks == 1) {
            printf("Sequencia ordenada: \n");
            printArray(sequencia,n_sequencia);

            /*escreve sequencia para o ficheiro*/
            if (writeInts(path, sequencia, n_sequencia) < 0) {
                perror("Reading file");
                exit(0);
            }
            exit(EXIT_SUCCESS);
        }



        /*divide o novo array por blocos sendo a nova
        divisao metade dos blocos anterior*/
        n_blocks = n_blocks / 2;
        /*atualiza numero de inteiros por bloco conforme o numero de blocos que divide a sequencia difere*/
        ints_per_block = n_sequencia / n_blocks;

    }






    /*
    quando o filho tiver ordenado o bloco, escreve para o pai via pipe a msg[]="#pid*index_start;index_end*sequencia|"
    envia sinal SIGURSI a pai
    pai recebe a msg de acordo com o protocolo retira o index start e o index end e coloca a sequencia agora ordenada de volta ao seu sitio
    quando todos os filhos tiverem retornado ao pai , formando um array completo ordenado no seu respetivo bloco
    volta a divir a sequencia em N blocos N = N_anterior / 2
    quando o N for menor que 1 significa que toda a sequencia esta ordenada
    armazena a sequencia de volta ao ficheiro*/


}

