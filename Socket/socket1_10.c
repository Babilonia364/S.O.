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
#include <semaphore.h>								/* Incluindo a biblioteca do linux para semaforos */


#define PORT 8080
#define BEGIN 9999


int main(int argc, char const *argv[])
{
	pid_t pid;
	sem_t *sync;
	clock_t tempo1, tempo2;
	
	struct sockaddr_in address;
	int32_t buffer[3], num[3], soma;
	int i;
	
	sync = sem_open("sSem", O_CREAT | O_EXCL, 0644, 1);
	
	tempo1=clock();
	for(i=0; i<10; i++)
	{
		if((pid=fork())<0)
		{
			perror("fork");
			exit(1);
		}
		if(pid==0)
			break;
	}
	
	
	/* Servidor do socket */
	if(pid>0)
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
		
		printf("Servidor dando listen\n");
		if (listen(server_fd, 100) < 0) 
		{
			perror("listen"); 
			exit(EXIT_FAILURE); 
		}
		
		
		for(i=0; i<10; i++)
		{
			/* Extrai a primeira conexao de conexoes pendentes na fila */
			if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
							(socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			}
				valread = read( new_socket , buffer, 3*sizeof(int)); 						/* Recebendo mensagem do cliente */
				printf("buffer: %d\n", buffer[0]);
			printf("buffer[0]: %d\nbuffer[1]: %d\n", buffer[0], buffer[1]);
			num[0]=buffer[0]+buffer[1];
			send(new_socket , num , (sizeof(num)) , 0 );									/* Enviando mensagem de volta para o cliente */
			printf("Soma enviada\n");
		}
		wait(NULL);
		sem_unlink ("sSem");
		sem_close (sync);
		shutdown (new_socket, 2);
		tempo2=clock();
		printf("Tempo: %lf\n", (tempo2 - tempo1)/(double)CLOCKS_PER_SEC);
	}
	
	/* Cliente do socket */
	else
	{
	int sock = 0, valread; 
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
	printf("Numeros a serem somados %d + %d\n", num[0], num[1]);
	
	/* Conecta o socket */
	printf("Clente conectando %d\n", i);
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed %d\n", i);
		return -1;
	} 
	
	send(sock , num , (sizeof(num)) , 0 ); 								/* Enviando mensagem para o servidor */
	printf("Numeros enviados\n"); 
	valread = read( sock , buffer, sizeof(int)); 						/* Recebendo mensagem do servidor */
	printf("Soma: %d\n",buffer[0] );
	close (sock);
	sem_post(sync);
	}
}
