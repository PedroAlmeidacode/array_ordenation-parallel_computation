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

    /* Descritor de ficheiros para as duas pontas do pipe*/
    int fd[2];

    int pids[n_childs]; //variavel que vai conter o pid de todos os processos criados pelo processo pai para ordenar o array


    /*Cria o pipe*/
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(1);
    }


    int n_pids = 0, start_point =0, low = 0, high =0;


    /*Enquanto o array nao estiver organizado
    o segundo ciclo renova-se, ou seja o i volta a ser 0 e o numero de
    filhos/blocos é recalculado */
    while (1) {

        int i = 0;

        /*itera as vezes correspondentes ao numero de
        blocos que o array foi dividido*/
        while (i < n_blocks) {
            //criar filhos
            if ((pids[n_pids] = fork()) == -1) {
                perror("fork");
                exit(-1);
            }
            if (pids[n_pids] == 0) {
                printf("execucao filho numero: %d \t pid: %d\n",n_pids+1,getpid());
                low = low + ;
                high = ;

                mergesort_run(sequencia,n_sequencia,low,high);


                exit(EXIT_SUCCESS);
            }
            i++;
            n_pids++;
        }


        for (int j = 0; j < n_blocks; j++) {
            //esperar pelos filhos
            waitpid(pids[j+start_point], NULL, 0);
        }
        start_point = n_pids; // atuaiza o start points para que da proxima vez que esperar pelos filhos comece no id correto


        /*se tiver 1 na variavel n_blcks neste ponto do codigo significa
    que o array esta organizado */
        if (n_blocks == 1) {
            printf("Sequencia ordenada: \n");
            printArray(sequencia,n_sequencia);
            exit(EXIT_SUCCESS);
        }

        /*divide o novo array por blocos sendo a nova
        divisao metade dos blocos anterior*/
        n_blocks = n_blocks / 2;

    }






    /*
    cria um ciclo que lanca os filhos que vao ordenar os inteiros do seu bloco usando o merge sort
    para cada filho sera calculado o index_start e o index_end de forma a retirar o bloco respetivo a ordenar da 		sequencia
    quando o filho tiver ordenado o bloco, escreve para o pai via pipe a 					msg[]="#pid*index_start;index_end*sequencia|"
    envia sinal SIGURSI a pai
    pai recebe a msg de acordo com o protocolo retira o index start e o index end e coloca a sequencia agora ordenada de volta ao seu sitio
    quando todos os filhos tiverem retornado ao pai , formando um array completo ordenado no seu respetivo bloco
    volta a divir a sequencia em N blocos N = N_anterior / 2
    quando o N for menor que 1 significa que toda a sequencia esta ordenada
    armazena a sequencia de volta ao ficheiro*/


}

