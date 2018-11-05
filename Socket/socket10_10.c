#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <fcntl.h>

#define PORT 8080 


int main(int argc, char const *argv[])
{
	pid_t pid;
	

	key_t shmkey;                 /*      shared memory key       */
    int shmid;                    /*      shared memory id        */
    sem_t *sem;                   /*      synch semaphore         */
    int *p;


    shmkey = ftok ("/dev/null", 5);       /* valid directory name and a number */
    printf ("shmkey for p = %d\n", shmkey);
    shmid = shmget (shmkey, sizeof (int), 0644 | IPC_CREAT);
    if (shmid < 0){                           /* shared memory error check */
        perror ("shmget\n");
        exit (1);
    }

    p = (int *) shmat (shmid, NULL, 0);   /* attach p to shared memory */
    *p = 0;

    sem = sem_open ("pSem", O_CREAT | O_EXCL, 0644, 1);


	if((pid=fork())<0)
	{
		perror("fork");
		exit(1);
	}

	if(pid>0)
	{
		for(int i=0;i<10;i++)
		{
			pid_t pid;
			if((pid=fork())<0)
				{
					perror("fork");
					exit(1);
				}


			if(pid==0)
			{
				sem_wait (sem);
				
				int server_fd, new_socket, valread; 
			   	struct sockaddr_in address; 
		    	int opt = 1; 
		    	int addrlen = sizeof(address); 
		    	char buffer[1024] = {0}; 
		    	char hello[50];
				sprintf(hello,"server %d ID %d has sent a message to",i,getpid());
		       
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
		    	for(int k = 0;k<10;k++)
		    	{ 
			    	if (listen(server_fd, 3) < 0) 
			    	{ 
			        	perror("listen"); 
			        	exit(EXIT_FAILURE); 
			    	} 
			    	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
			                       (socklen_t*)&addrlen))<0) 
			    	{ 
			        	perror("accept"); 
			        	exit(EXIT_FAILURE); 
			    	} 
			    	
				    valread = read( new_socket , buffer, 1024);
				    printf("%s server %d\n",buffer,i ); 
				    send(new_socket , hello , strlen(hello) , 0 ); 
				    //printf("Hello message sent from server\n");
			    }
			    sem_post (sem);
			    exit(0);
			}
		}

	}
	else if(pid==0)
	{
		

		pid_t pid;
		for(int i=0;i<10;i++)
		{
			if((pid=fork())<0)
				{
					perror("fork");
					exit(1);
				}

			if(pid==0)
			{
				struct sockaddr_in address; 
				int sock = 0, valread; 
				struct sockaddr_in serv_addr;
				int j=i+48;
				char hello[50];
				sprintf(hello,"client %d ID %d has sent a message to",i,getpid());
				char buffer[1024] = {0};
				if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
				{ 
					printf("\n Socket creation error \n"); 
					return -1; 
				} 
				for (int k = 0;k<0;k++)
				{
					memset(&serv_addr, '0', sizeof(serv_addr)); 

					serv_addr.sin_family = AF_INET; 
					serv_addr.sin_port = htons(PORT); 
					
					// Convert IPv4 and IPv6 addresses from text to binary form 
					if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
					{ 
						printf("\nInvalid address/ Address not supported \n"); 
						return -1; 
					} 

					if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
					{ 
						printf("\nConnection Failed\n"); 
						return -1; 
					}
					send(sock , hello , strlen(hello) , 0 ); 
					//printf("Hello message sent from client %d ID %d\n",i,getpid()); 
					valread = read( sock , buffer, 1024); 
					printf("%s client %d\n",buffer,i );
					*p++;
					while(*p%10){}

				
				}

				shmdt (p);
		        shmctl (shmid, IPC_RMID, 0);

		        /* cleanup semaphores */
		        sem_unlink ("pSem");   
		        sem_close(sem);

				exit(0);			
			}
		}
	}
}