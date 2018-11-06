#include <stdio.h>									/*					printf()					*/
#include <stdlib.h>									/*			exit(), malloc(), free()			*/
#include <sys/types.h>								/*				sem_t, pid_t, key_t				*/
#include <sys/wait.h>								/*					waitpid						*/
#include <unistd.h>
#include <errno.h>									/*					errno, ECHILD				*/
#include <semaphore.h>								/* Incluindo a biblioteca do linux para semaforos */
#include <fcntl.h>									/*				O_CREAT, O_EXEC					*/
#include "pipe.h"

#define BUFFER 256									/* Tamanho do buffer dos file descriptor */
#define P1P1 1										/* Fazendo 1 processo se comunicar com 1 */
#define P10P1 2										/* 10 processos se comunicando com 1 processo */
#define P10P10 3									/* 10 processos se comunicando com 10 processos */

/* Compilar usando gcc pipe.c -lpthread -lrt -o pipe && ./pipe para poder rodar a biblioteca de semaphoro */



int main()
{
	int fd1[2], fd2[2];								/* Canais de escrita e leitura */
	int  processos, i;								/* Variavel para saber quantos processos operam e um contador para os processos */
	pid_t pid;										/* Pid dos processos */

	/*Definindo quantos processos se comunicarao entre si*/
	printf("Quantos procesos voce deseja comunicar entre si?\n"
			"Digite os respectivos inteiros de cada operacao\n"
			"|\t1 - Um processo se comunica com um processo\t\t|\n"
			"|\t2 - Dez processos se comunicam com um processo\t\t|\n"
			"|\t3 - Dez processos se comunicam com dez processos\t|\n");
	scanf("%d", &processos);
	
	
	/* Criando o primeiro pipao da massa */
	if(pipe(fd1)<0)
	{
		perror("pipe");
		return -1;
	}

	/* Criando o segundo pipao da masssa */
	if(pipe(fd2)<0)
	{
        perror("pipe");
        return -1;
    }

	switch(processos)
	{
    /* Criando os processos da massa */
		case P1P1:
			if((pid=fork())<0)					/* Fazendo fork de apenas um processo e tratando erro */
			{
				perror("fork");
				exit(1);
			}
			pipeUmUm(pid, fd1, fd2);			/* Entra na funcao para passar dados */
			break;
		case P10P1:
		{
			sem_t *produtor;					/* Semaforo compartilhado entre os processos produtores */
			
			/* Iniciando semaforo compartilhado na memória */
			produtor = sem_open("pSem", O_CREAT | O_EXCL, 0644, 1);
			
			
			for(i=0; i<10; i++)					/* Criando um laco para fazer 10 processos apenas */
			{
				printf("Criando processo %d\n", i);
				if((pid=fork())<0)
				{
					perror("fork");
					exit(1);
				}
				if(pid==0)						/* se for um processo filho, sair do laco */
					break;
			}
			if(pid==0)
			{
				sem_wait(produtor);				/* Entrando na condicao de corrida */
				printf("Processo %d entrando na condicao de corrida\n", i);
				pipeDezUm(pid, fd1, fd2, i);
				sem_post(produtor);
			}else
			{
				pipeDezUm(pid, fd1, fd2, i);
				sem_unlink ("pSem");			/* Desligando o semaforo */
				sem_close (produtor);			/* Fechando o semaforo */
				exit(0);
			}
			break;
		}
		
		case P10P10:
		{
			sem_t *produtor;					/* Semaforo compartilhado entre os processos produtores */
			sem_t *consumidor;					/* Semaforo compartilhado entre os processos consumidores */
			sem_t *mutex;
			int k=0;
			
			/* Iniciando semaforo compartilhado na memória */
			produtor = sem_open("pSem", O_CREAT | O_EXCL, 0644, 1);
			consumidor = sem_open("cSem", O_CREAT | O_EXCL, 0322, 1);
			mutex = sem_open("mSem", O_CREAT | O_EXCL, 1244, 0);
			
			for(i=0; i<20; i++)						/* Criando 9 processos filhos, para totalizar 10 */
			{
				if((pid=fork())<0)
				{
					perror("fork");
					exit(1);
				}
				if(pid==0)							/* Caso seja um processo filho, saia do laco */
					break;
			}
			if(pid>0)
			{
				/* Esperando todos os filhos saire, afinal, o ultimo processo a sair sera um filho */
				while(pid=waitpid(-1, NULL, 0))	/* Espera pelo processo término do processo filho até que */
				{
					if(errno == ECHILD)			/* Nenhum processo filho exista mais, entao errno e atualizado com ECHILD */
						break;
				}
				printf("Todos os filhos sairam\n");
				
				sem_unlink ("pSem");			/* Desligando o semaforo */
				sem_unlink ("cSem");
				sem_unlink ("mSem");
				sem_close (produtor);			/* Fechando o semaforo */
				sem_close(consumidor);
				sem_close(mutex);
				exit(0);
			}else
			{
				if(i<10)
				{
					close(fd1[0]);								/* Fechando os canais para escrita e leitura */
					close(fd2[1]);
					
					while(k<10)
					{
						sem_wait(produtor);						/* Produtor entrando na zona critica */
						pipeDezDez(pid, mutex, consumidor, fd1, fd2, i);
						sem_post(produtor);						/* Produtor saindo da zona critica */
						k++;
					}
					
					/* fechando os arquivinhos restantes */
					close(fd1[1]);
					close(fd2[0]);
					
				}else
				{
					
					close(fd1[1]);								/* Neste caso, o processo pai vai ler do primeiro e escrever no segundo */
					close(fd2[0]);								/* Logo, deixa se o fd1[0] aberto para ler e o fd2[1] aberto para escrever */
					
					//sem_wait(consumidor);
					pipeDezDez(pid, mutex, consumidor, fd1, fd2, i);
					//sem_post(consumidor);
					
					close(fd1[0]);								/* Fechando os arquivos para leitura e escrita */
					close(fd2[1]);
				}
			}
			break;
		}
	}

}
