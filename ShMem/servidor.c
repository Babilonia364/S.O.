#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include<unistd.h> 	/* Usada apenas para habilitar o comando sleep */

int main()
{
	key_t chave;	/* Chave a ser passada para a shmget() */
	int shmFlag;	/* Flag a ser passada para shmget */
	int shmId;		/* Id da area de memoria retornada pela shmget */
	int tam=27;		/* Tamanho em bytes que é requerido para a memoria compartilhada  */
	char *mem, c, *s;

	chave = ftok("shmfile", 65);	/* ftok() gera uma chave unica */
	printf("Chave gerada: %d\n", chave);

	if((shmId=shmget(chave, tam, 0666|IPC_CREAT))<0)		/* Criando espaco de memoria e retornando seu id */
	{
		printf("Erro na criacao do espaco de memoria\n");
		return 0;
	}

	if((mem=(char*)shmat(shmId, NULL, 0))==(char*)-1)				/* Antes de poder usar a memoria compartilhada, anexamos ela usando shmat */
	{
		printf("Erro ao anexar memoria\n");
		return 0;
	}

	/* Colocando o alfabeto na memoria para outro processo ler */
	s=mem;
	printf("Escrevenu na memoria: ");
	for(c='a'; c<='z'; c++)
	{
		*s++=c;
		printf("%c ", c);
	}
		printf("\nFim da escrita\n");

	/* Esperando ate o outro processo confirmar que leu colocando '*' como o primeiro caractere na memoria */
	while(*mem!='*')
		sleep(1);

	shmdt(mem);		/* Desanexando da memoria */

	return 0;
}
