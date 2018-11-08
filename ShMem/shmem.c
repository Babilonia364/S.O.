#include <stdio.h>								/*					printf					*/
#include <stdlib.h>								/*					exit()					*/
#include <sys/types.h>							/*			key_t, pid_t, sem_t				*/
#include <sys/ipc.h>							/*			IPC_CREAT, IPC_RMID				*/
#include <sys/shm.h>							/*					shmat()					*/
#include <sys/wait.h>							/*				wait, waitpid				*/
#include <unistd.h>								/*					sleep()					*/
#include <errno.h>								/*				errno, ECHILD				*/
#include <semaphore.h>							/* Incluindo a biblioteca do linux para semaforos */
#include <fcntl.h>								/*				O_CREAT, O_EXEC					*/
#include "shmem.h"

#define P1P1 1										/* Fazendo 1 processo se comunicar com 1 */
#define P10P1 2										/* 10 processos se comunicando com 1 processo */
#define P10P10 3									/* 10 processos se comunicando com 10 processos */

/* Compilar usando gcc pipe.c -lpthread -lrt -o pipe && ./pipe para poder rodar a biblioteca de semaphoro */

int main()
{
	key_t chave;					/* Chave a ser passada para a shmget() */
	pid_t pid;						/* Variavel onde ficara armazenado o pid dos processos */
	int shmId;						/* Id da area de memoria retornada pela shmget */
	int tam=30*sizeof(int);			/* Tamanho em bytes que é requerido para a memoria compartilhada  */
	int *mem, c, *s;
	int processos, i;

	printf("Quantos procesos voce deseja comunicar entre si?\n"
			"Digite os respectivos inteiros de cada operacao\n"
			"|\t1 - Um processo se comunica com um processo\t\t|\n"
			"|\t2 - Dez processos se comunicam com um processo\t\t|\n"
			"|\t3 - Dez processos se comunicam com dez processos\t|\n");
	scanf("%d", &processos);
	
	chave = ftok("shmfile", 65);

	/* Criando o espaco de memoria usando shmget e retornando seu id para shmId */
	if((shmId=shmget(chave, tam, 0644|IPC_CREAT))<0)
	{
		perror("shmget\n");
		exit(1);
	}

	/* Antes de poder usar a memoria compartilhada, anexamos ela usando shmat */
	if((mem=(int*)shmat(shmId, NULL, 0))==(int*)-1)
	{
		perror("shmat\n");
		exit(1);
	}

	switch(processos)
	{
		case P1P1:
			/* Fazendo um fork e tratando erro caso o fork nao ocorra */
			if((pid=fork())<0)
			{
				perror("fork");
				exit(1);
			}
			memUmUm(pid, s, c, mem);
			
			if(pid>0)
			{
				shmdt(mem);						/* Desanexando da memoria */
				shmctl(shmId, IPC_RMID, NULL);	/* Destruindo memória compartilhada */
			}
			
		break;
		
		case P10P1:
		{
			sem_t *sync1;						/* Declarando semaforo */
			sem_t *sync2;
			sem_t *produtor;
			
			/* Iniciando semaforo para sincronizar os processos */
			sync1 = sem_open("s1Sem", O_CREAT | O_EXCL, 0644, 0);
			sync2 = sem_open("s2Sem", O_CREAT | O_EXCL, 0322, 0);
			produtor = sem_open("pSem", O_CREAT | O_EXCL, 1288, 1);
			
			/* Criando 10 processos filhos que enviarao mensagem para o processo pai */
			for(i=0; i<10; i++)
			{
				if((pid=fork())<0)				/* Caso o retorno do fork seja -1, deu erro */
				{
					perror("fork");
					exit(1);
				}
				if(pid==0)						/* Caso seja um processo filho, sai do laco assim gera somente 10 processos filhos */
					break;
			}
			
			if(pid>0)
			{
				memDezUm(pid, sync1, sync2, s, c, mem, i);
				
				printf("Apagando as paradas\n");
				shmdt(mem);						/* Desanexando da memoria */
				shmctl(shmId, IPC_RMID, NULL);	/* Destruindo memória compartilhada */
				sem_unlink ("s1Sem");			/* Desligando o semaforo */
				sem_unlink ("s2Sem");
				sem_unlink ("pSem");
				sem_close (sync1);				/* Fechando o semaforo */
				sem_close (sync2);
				sem_close (produtor);
			}else
			{
				sem_wait(produtor);
				printf("produtor entrou na zona critica\n");
				memDezUm(pid, sync1, sync2, s, c, mem, i);
				sem_post(produtor);
				printf("produtor saiu da zona critica\n");
			}
			
			break;
		}
		
		case P10P10:
		{
			sem_t *sync1;						/* Declarando semaforo */
			sem_t *sync2;
			sem_t *produtor;
			sem_t *consumidor;
			int j;
			
			/* Iniciando semaforo para sincronizar os processos */
			sync1 = sem_open("s1Sem", O_CREAT | O_EXCL, 0644, 0);
			sync2 = sem_open("s2Sem", O_CREAT | O_EXCL, 0322, 0);
			produtor = sem_open("pSem", O_CREAT | O_EXCL, 1288, 1);
			consumidor = sem_open("cSem", O_CREAT | O_EXCL, 0161, 1);
			
			for(i=0; i<20; i++)
			{
				/* Criando 20 processos filhos para se comunicarem entre si */
				if((pid=fork())<0)
				{
					perror("fork\n");
					exit(1);
				}
				
				if(pid==0)
					break;
			}
			
			if(pid>0)
			{
				while(pid=waitpid(-1, NULL, 0))
				{
					if (errno == ECHILD)
						break;
				}
				printf("Apagando as paradas\n");
				
				shmdt(mem);						/* Desanexando da memoria */
				shmctl(shmId, IPC_RMID, NULL);	/* Destruindo memória compartilhada */
				sem_unlink ("s1Sem");			/* Desligando o semaforo */
				sem_unlink ("s2Sem");
				sem_unlink ("pSem");
				sem_unlink ("cSem");
				sem_close (sync1);				/* Fechando o semaforo */
				sem_close (sync2);
				sem_close (produtor);
				sem_close (consumidor);
			}else
			{
				if(i<10)
				{
					sem_wait(consumidor);
					printf("consumidor entrou na zona critica");
					memDezDez(pid, sync1, sync2, s, c, mem, i);
					sem_post(consumidor);
					printf("consumidor saiu da zona critica\n");
				}else
				{
					for(j=0; j<10; j++)
					{
						sem_wait(produtor);
						printf("produtor entrou na zona critica\n");
						memDezDez(pid, sync1, sync2, s, c, mem, i);
						sem_post(produtor);
						printf("produtor saiu da zona critica\n");
					}
				}
			}
			break;
		}
	}
	
	return 0;
}
