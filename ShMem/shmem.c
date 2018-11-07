#include <stdio.h>								/*					printf					*/
#include <stdlib.h>								/*					exit()					*/
#include <sys/types.h>							/*			key_t, pid_t, sem_t				*/
#include <sys/ipc.h>							/*			IPC_CREAT, IPC_RMID				*/
#include <sys/shm.h>							/*					shmat()					*/
#include <sys/wait.h>							/*				wait, waitpid				*/
#include <unistd.h>								/*					sleep()					*/
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
	int processos;

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
	}
	
	return 0;
}
