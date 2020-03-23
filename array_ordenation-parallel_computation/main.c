#include "library.h"




int main(int argc, char *argv[] )
{

	/*inicializa dinamicamente sequencia de inteiros a ordenar com um tamanho definido por N_MAX*/
	int *sequencia = newIntArray(N_MAX);
	int n_sequencia;

	/*processo pai le path de *argv ,input do terminal*/
	char path[100];
	printf("\nIntroduza o camimho do ficheiro:\n");
    scanf("%99[^\n]%*c", path);

	/*pai le inteiros contidos no ficheiro e armazena no array*/
	n_sequencia = readInts(path,sequencia,N_MAX);
	if(n_sequencia < 0 )
	{
		perror("Reading file");
		exit(0);
	}

	/*depois de saber numero de inteiros a colocar no array da free e reneicializa com o seu tamanho exato*/
	free(sequencia);
	sequencia = newIntArray(n_sequencia);

	/*imprime sequencia desordenada*/
	printf("Sequencia desordenada:\n");
	printArray(sequencia,n_sequencia);

	/*define numero de blocos a dividir array inicial, numeros de filhos a lancar para primeira operação*/
	int n_blocks = numberOfBlocksToDivideArray(n_sequencia);

	/* Descritor de ficheiros para as duas pontas do pipe*/
	int fd[2];
	
	
    	/*Cria o pipe*/
	if(pipe(fd)== -1)
	{
		perror("pipe");
		exit(1);
	}



	
	int array_organizado =0;

	/*Enquanto o array nao estiver organizado(a variavel nao for 1), 
	o segundo ciclo renova-se, ou seja o i volta a ser 0 e o numero de
	filhos/blocos é recalculado */
	while(array_organizado != 1)
	{
		int i=0;
		/*itera as vezes correspondentes ao numero de 
		blocos que o array foi dividido*/
		while(i < n_blocks)
		{

			i++;
		}
		
		/*se tiver 1 na variavel n_blcks neste ponto do codigo significa 
		que o array esta organizado */
		if(n_blocks == 1) array_organizado = 1;

		/*divide o novo array por blocos sendo a nova 
		divisao metade dos blocos anterior*/
		n_blocks = n_blocks / 2;
		
	}











	/*
	cria um ciclo que lanca os filhos que vao ordenar os inteiros do seu bloco usando o merge sort
	para cada filho sera calculado o index_start e o index_end de forma a retirar o bloco respetivo a ordenar da 		sequencia 
	quando o filho tiver ordenado o bloco, escreve para o pai via pipe a 					msg[]="#pid*index_start;index_end*sequencia|"
	pai recebe a msg de acordo com o protocolo retira o index start e o index end e coloca a sequencia agora ordenada de volta ao seu sitio
	quando todos os filhos tiverem retornado ao pai , formando um array completo ordenado no seu respetivo bloco
	volta a divir a sequencia em N blocos N = N_anterior / 2 
	quando o N for menor que 1 significa que toda a sequencia esta ordenada
	armazena a sequencia de volta ao ficheiro

    	ssize_t n;
    	int i = 0;
    	int pids[n_filhos];
	int index_start, index_end;
	char *sequencia;

	
		


    /* Create a pipe.  File descriptors for the two ends of the pipe are
       placed in fds.  */
	int fd[2];
	
	

    /*
    * Create the pipe
    */
	if(pipe(fd)== -1){
		perror("pipe");
		exit(1);
	}

    /* Fork a child process.  */
    for (i = 0; i < 2; i++)
    {

        //criar filhos
	if((pids[i] = fork()) == -1){
			perror("fork");exit(-1);}

        if (pids[i] == 0)
        {
            int f;
            char *token;
            char buf[BUFFSIZE];

            /*fecho dos fd que o filho nao vai usar.*/
		close(fd[0]);

            f = open("file.txt", O_CREAT | O_RDWR, 0644);

            while ((n = read(f, buf, BUFFSIZE)) != 0)
            {
                token = strtok(buf, "\n");
                while (token != NULL)
                {
		char msg[200];
		sprintf(msg,"%d*%s#\n",getpid(),token);
		//escrever para o pipe
		write(fd[1],msg,strlen(msg));
                    
                    token = strtok(NULL, "\n");
                }
            }

            /*fecho dos fd que já não sao necessarios*/
		close(fd[1]);
            exit(EXIT_SUCCESS);
        }
    }

    //main program.....
    
    /*fecho dos fd que o filho nao vai usar.*/
	close(fd[1]);
    
    char buffer[BUFFSIZE];
    while ((n = read(fd[0], buffer, BUFFSIZE)) > 0)
        //escrever para o stdout
	write(1, buffer,n);

    for (i = 0; i < 2; i++)
    {
        //esperar pelos filhos
	waitpid(pids[i],NULL,0);
    }

    exit(EXIT_SUCCESS);
    printf("DONE!\n");


}
