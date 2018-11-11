#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#include <fcntl.h>									/*				O_CREAT, O_EXEC					*/
#include <stdint.h>									/*					int32_t						*/
#include <time.h>									/*				clock_t, clock()				*/
#include <sys/wait.h>								/*				wait, waitpid					*/
#include <errno.h>									/*					errno, ECHILD				*/
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <semaphore.h>								/* Incluindo a biblioteca do linux para semaforos */


#define PORT 8080
#define END 9999


int main(int argc, char const *argv[])
{
	pid_t pid;
	key_t shmkey;
	sem_t *sync, *sync2;
	clock_t tempo1, tempo2;
	
	struct sockaddr_in address;
	int32_t buffer[3], num[3], soma;
	int i, j;
	int *p;
	int shmid;
	
	sync = sem_open("sSem", O_CREAT | O_EXCL, 0644, 1);
	sync2 = sem_open("s2Sem", O_CREAT | O_EXCL, 0322, 1);
	
	/* Inicializando variavel compartilhada na memoria */
	shmkey = ftok ("/dev/null", 5);
	
	shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);
	
	p = (int *) shmat (shmid, NULL, 0);				/* Anexando p na memoria compartilhada */
	*p = 0;
	
	tempo1=clock();
	for(i=0; i<20; i++)
	{
		if((pid=fork())<0)
		{
			perror("fork");
			exit(1);
		}
		if(pid==0)
			break;
	}
	
/***
 *          ___           ___           ___           ___           ___           ___     
 *         /\  \         /\  \         /\  \         /\__\         /\  \         /\  \    
 *        /::\  \       /::\  \       /::\  \       /:/  /        /::\  \       /::\  \   
 *       /:/\ \  \     /:/\:\  \     /:/\:\  \     /:/  /        /:/\:\  \     /:/\:\  \  
 *      _\:\~\ \  \   /::\~\:\  \   /::\~\:\  \   /:/__/  ___   /::\~\:\  \   /::\~\:\  \ 
 *     /\ \:\ \ \__\ /:/\:\ \:\__\ /:/\:\ \:\__\  |:|  | /\__\ /:/\:\ \:\__\ /:/\:\ \:\__\
 *     \:\ \:\ \/__/ \:\~\:\ \/__/ \/_|::\/:/  /  |:|  |/:/  / \:\~\:\ \/__/ \/_|::\/:/  /
 *      \:\ \:\__\    \:\ \:\__\      |:|::/  /   |:|__/:/  /   \:\ \:\__\      |:|::/  / 
 *       \:\/:/  /     \:\ \/__/      |:|\/__/     \::::/__/     \:\ \/__/      |:|\/__/  
 *        \::/  /       \:\__\        |:|  |        ~~~~          \:\__\        |:|  |    
 *         \/__/         \/__/         \|__|                       \/__/         \|__|    
 */
	
	if(pid>0)
	{
		while(pid=waitpid(-1, NULL, 0))	/* Espera pelo processo término do processo filho até que */
		{
			if(errno == ECHILD)			/* Nenhum processo filho exista mais, entao errno e atualizado com ECHILD */
				break;
		}
		printf("Limpando tudo\n");
		
		tempo2=clock();
		printf("Tempo: %lf\n", (tempo2 - tempo1)/(double)CLOCKS_PER_SEC);
		
		sem_unlink ("sSem");
		sem_close (sync);
		
		/* Desanexando e limpando a variavel de memoria */
        shmdt (p);
        shmctl (shmid, IPC_RMID, 0);
	}
	else if(i<10)
	{
		int server_fd, new_socket, valread; 
		int opt = 1; 
		int addrlen = sizeof(address);
		
		
		// Creating socket file descriptor 
		if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
		{ 
			perror("socket failed"); 
			exit(EXIT_FAILURE);
		} 
		
		// Forcefully attaching socket to the port 8080 
		if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
													&opt, sizeof(opt))) 
		{ 
			perror("setsockopt"); 
			exit(EXIT_FAILURE); 
		} 
		address.sin_family = AF_INET; 
		address.sin_addr.s_addr = INADDR_ANY; 
		address.sin_port = htons( PORT ); 
		
		// Forcefully attaching socket to the port 8080 
		if (bind(server_fd, (struct sockaddr *)&address,  
									sizeof(address))<0) 
		{ 
			perror("bind failed"); 
			exit(EXIT_FAILURE); 
		}
		
		/* Dando listen, esperando por conexoes no socket ja criado */
		
		printf("Servidor %d dando listen\n", i);
		
		*p = (*p)+1;
		printf("p: %d\n", *p);
		
		if (listen(server_fd, 10) < 0) 
		{
			perror("listen"); 
			exit(EXIT_FAILURE); 
		}
		
		for(j=0; j<10; j++)
		{
			//sem_wait(sync2);
			/* Extrai a primeira conexao de conexoes pendentes na fila */
			if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
							(socklen_t*)&addrlen))<0) 
			{ 
				perror("accept");
				exit(EXIT_FAILURE); 
			}
			
			printf("Servidor %d lendo\n", i);
			valread = read( new_socket , buffer, 3*sizeof(int)); 							/* Recebendo mensagem do cliente */
			num[0]=buffer[0]+buffer[1];
			send(new_socket , num , (sizeof(num)) , 0 );									/* Enviando mensagem de volta para o cliente */
			printf("Soma enviada\n");
			//sem_post(sync2);
		}
		
		shutdown (new_socket, 2);
		printf ("Processo %d fechando depois de %d execucoes\n", i, j);
	}
	
/***
 *          ___           ___                   ___           ___           ___     
 *         /\  \         /\__\      ___        /\  \         /\__\         /\  \    
 *        /::\  \       /:/  /     /\  \      /::\  \       /::|  |        \:\  \   
 *       /:/\:\  \     /:/  /      \:\  \    /:/\:\  \     /:|:|  |         \:\  \  
 *      /:/  \:\  \   /:/  /       /::\__\  /::\~\:\  \   /:/|:|  |__       /::\  \ 
 *     /:/__/ \:\__\ /:/__/     __/:/\/__/ /:/\:\ \:\__\ /:/ |:| /\__\     /:/\:\__\
 *     \:\  \  \/__/ \:\  \    /\/:/  /    \:\~\:\ \/__/ \/__|:|/:/  /    /:/  \/__/
 *      \:\  \        \:\  \   \::/__/      \:\ \:\__\       |:/:/  /    /:/  /     
 *       \:\  \        \:\  \   \:\__\       \:\ \/__/       |::/  /     \/__/      
 *        \:\__\        \:\__\   \/__/        \:\__\         /:/  /                 
 *         \/__/         \/__/                 \/__/         \/__/                  
 */
	
	else
	{
		while(*p<9)
			sleep(1);
		for(j=0; j<10; j++)
		{
			int sock = 0, valread, sair=0; 
			struct sockaddr_in serv_addr; 
			
			if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{ 
				printf("\n Socket creation error \n");
				return -1; 
			} 
			
			memset(&serv_addr, '0', sizeof(serv_addr)); 
			
			serv_addr.sin_family = AF_INET; 
			serv_addr.sin_port = htons(PORT); 
			
			// Convert IPv4 and IPv6 addresses from text to binary form 
			if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
			{ 
				printf("\nInvalid address/ Address not supported \n"); 
				return -1; 
			} 
			
			/* Definindo numeros */
			sem_wait(sync);
			num[0]=i+1;
			num[1]=i;
			
			/* Conecta o socket */
			printf("Clente conectando %d\n", i);
			while ((connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)&&(sair<5))
			{
				printf("\nConnection Failed %d\n", i);
				sair++;
				sleep(1);
				//return -1;
			}
			
			send(sock , num , (sizeof(num)) , 0 ); 								/* Enviando mensagem para o servidor */
			printf("Numeros enviados\n"); 
			valread = read( sock , buffer, sizeof(int)); 						/* Recebendo mensagem do servidor */
			printf("Soma: %d\n",buffer[0] );
			close (sock);
			sem_post(sync);
		}
	}
}
