#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
	key_t chave;	/* Chave a ser passada para a shmget() */
	int shmId;	/* Id da area de memoria retornada pela shmget */
	int tam=27;	/* Tamanho em bytes que é requerido para a memoria compartilhada  */
	char *mem, c, *s;

	chave = ftok("shmfile", 65);

	if((shmId=shmget(chave, tam, 0666|IPC_CREAT))<0)		/* Tratamento de erro */
	{
		printf("Erro ao acessar espaco de memoria\n");
		return 0;
	}

	if((mem=(char*)shmat(shmId, NULL, 0))==(char*)-1)		/* Conversoes explicitas para char */
	{
		printf("Erro ao anexar espaco de memoria\n");
		return 0;
	}

	/* Hora de ler o que foi colocado na memoria */
	printf("Lendo o que foi escrito na mem: ");
	for(s=mem; *s!='\0'; s++)
	{
		printf("%c ", *s);
	}
	printf("\n");
	*mem='*';

	shmdt(mem);	/* Desanexando memoria */

	shmctl(shmId, IPC_RMID, NULL);	/* Destruindo memória compartilhada */

	return 0;
}
