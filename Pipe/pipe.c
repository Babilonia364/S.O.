#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>								/* Incluindo a biblioteca do linux para semaforos */
#include "pipe.h"

#define BUFFER 256									/* Tamanho do buffer dos file descriptor */
#define P1P1 1										/* Fazendo 1 processo se comunicar com 1 */
#define P10P1 2										/* 10 processos se comunicando com 1 processo */
#define P10P10 3									/* 10 processos se comunicando com 10 processos */

/* Compilar usando gcc pipe.c -lpthread -lrt -o pipe && ./pipe para poder rodar a biblioteca de semaphoro */

sem_t semaforo;										/* Criando o semaforo, como variavel global, para os processos nao entrarem na zona critica ao mesmo tempo */

int main()
{
	int fd1[2], fd2[2];								/* Canais de escrita e leitura */
	int  processos, i;								/* Variavel para saber quantos processos operam e um contador para os processos */
	pid_t pid;										/* Pidão dos processões da massa */

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
			if((pid=fork())<0)
			{
				perror("fork");
				exit(1);
			}
			pipeUmUm(pid, fd1, fd2);
			break;
		case P10P1:
			sem_init(&semaforo, 1, 1);				/* iniciando o semaforo, passando seu endereco, indicando que e um semaforo para processos e indicando seu valor inicial */
			for(i=0; i<10; i++)
			{
				printf("Criando processo %d\n", i);
				if((pid=fork())<0)
				{
					perror("fork");
					exit(1);
				}
				if(pid==0)
				{
					printf("Castrando filho %d\n", i);
					break;
				}
			}
			pipeDezUm(pid, fd1, fd2, semaforo, i);
		break;
	}
	sem_destroy(&semaforo);

}
