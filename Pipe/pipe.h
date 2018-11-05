void pipeUmUm(pid_t pid, int* fd1, int* fd2)
{
	int turn=0;											/* Variável que define o que cada processinho vai fazer */
	
	/***********************************************************************************************/
	/*.------..------..------..------..------..------..------..------.     .------..------..------.*/
	/*|P.--. ||R.--. ||O.--. ||C.--. ||E.--. ||S.--. ||S.--. ||O.--. |.-.  |P.--. ||A.--. ||I.--. |*/
	/*| :/\: || :(): || :/\: || :/\: || (\/) || :/\: || :/\: || :/\: ((5)) | :/\: || (\/) || (\/) |*/
	/*| (__) || ()() || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |'-.-.| (__) || :\/: || :\/: |*/
	/*| '--'P|| '--'R|| '--'O|| '--'C|| '--'E|| '--'S|| '--'S|| '--'O| ((1)) '--'P|| '--'A|| '--'I|*/
	/*`------'`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'*/
	/***********************************************************************************************/
	
	if(pid>0)
	{
		/* No process sugarDaddy, vamos escrever, então fechar a leitura desse lado */
		close(fd1[0]);
		close(fd2[1]);								/* fecha - se um canal para escrever e ler em cada documento */

		int num[2];									/* Numeros lidos pelo suggar daddy */
		int soma;									/* Soma recebida do sugginha */

		while(1)
		{
			if(turn==0)								/* Sugar Daddy vai escrever para o suggar kiddo */
			{
				printf("Digite o primeiro numero da soma sera 1");
				num[0]=1;
				printf("\nDigite o segundo numero da soma sera 2");
				num[1]=2;
				printf("\n");

				write(fd1[1], num, sizeof(num));
				turn=1;
			}
			else if(turn==1)						/* hora de ler a sominha */
			{
				read(fd2[0], &soma, sizeof(soma));	/* Armazenando o resultz que o sugginha mandou */

				printf("O resultz da soma sera: %d\n", soma);
				turn=0;								/* Hora de escrever dinovu 2 numeritos */
				break;
			}
		}

		/* fechando os arquivinhos restantes */
		close(fd1[1]);
		close(fd2[0]);
	}
	
	/***************************************************************************************************************/
	/*.------..------..------..------..------..------..------..------.     .------..------..------..------..------.*/
	/*|P.--. ||R.--. ||O.--. ||C.--. ||E.--. ||S.--. ||S.--. ||O.--. |.-.  |F.--. ||I.--. ||L.--. ||H.--. ||O.--. |*/
	/*| :/\: || :(): || :/\: || :/\: || (\/) || :/\: || :/\: || :/\: ((5)) | :(): || (\/) || :/\: || :/\: || :/\: |*/
	/*| (__) || ()() || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |'-.-.| ()() || :\/: || (__) || (__) || :\/: |*/
	/*| '--'P|| '--'R|| '--'O|| '--'C|| '--'E|| '--'S|| '--'S|| '--'O| ((1)) '--'F|| '--'I|| '--'L|| '--'H|| '--'O|*/
	/*`------'`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'`------'`------'*/
	/***************************************************************************************************************/
	
	else
	{
		close(fd1[1]);								/* O processo kiddo vai ler do primeiro e escrever no segundo */
		close(fd2[0]);								/* Logo, deixa se o fd1[0] aberto para ler e o fd2[1] aberto para escrever */

		int num2[2];
		int soma;


		while(1)
		{
			if(turn==0)								/* Hora de ler do fd1[0] para fazer a soma */
			{										/* Numeros vindo do pai */
				read(fd1[0], num2, sizeof(num2));	/* Recebeu os numeros de papai */
				turn=1;								/* Muda o turn para o efetuar o passo seguinte */
			}
			else if(turn==1)						/* Hora de efetuar a soma e mandar de volta para o suggar daddy */
			{										/* Numeros recebidos de papai */
				soma=num2[0]+num2[1];

				write(fd2[1], &soma, sizeof(soma));	/* Enviando a soma para o papis pelo pipe */
				turn=0;
				break;
			}
		}

		close(fd1[0]);								/* Fechando os arquivos para leitura e escrita */
		close(fd2[1]);
	}
}
void pipeDezUm(pid_t pid, int* fd1, int* fd2, int i)
{
	int turn=0;
	
	/***************************************************************************************************************/
	/*.------..------..------..------..------..------..------..------.     .------..------..------..------..------.*/
	/*|P.--. ||R.--. ||O.--. ||C.--. ||E.--. ||S.--. ||S.--. ||O.--. |.-.  |F.--. ||I.--. ||L.--. ||H.--. ||O.--. |*/
	/*| :/\: || :(): || :/\: || :/\: || (\/) || :/\: || :/\: || :/\: ((5)) | :(): || (\/) || :/\: || :/\: || :/\: |*/
	/*| (__) || ()() || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |'-.-.| ()() || :\/: || (__) || (__) || :\/: |*/
	/*| '--'P|| '--'R|| '--'O|| '--'C|| '--'E|| '--'S|| '--'S|| '--'O| ((1)) '--'F|| '--'I|| '--'L|| '--'H|| '--'O|*/
	/*`------'`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'`------'`------'*/
	/***************************************************************************************************************/
	
	if(pid==0)
	{
		/* Neste caso, temos o processo filho se comunicando com o papa */
		close(fd1[0]);
		close(fd2[1]);								/* fecha - se um canal para escrever e ler em cada documento */

		int num[2];									/* Numeros lidos pelo suggar daddy */
		int soma;									/* Soma recebida do sugginha */

		while(1)
		{
			
			if(turn==0)								/* Filinho querido vai escrever cartinha para papai */
			{
				printf("Processo %d pergunta\n", i);
				printf("Digite o primeiro numero da soma sera %d", i);
				num[0]=i;
				printf("\nDigite o segundo numero da soma sera %d", i+1);
				num[1]=i+1;
				printf("\n");

				write(fd1[1], num, sizeof(num));	/* Escrevendo o conteudo no fd1[1] */
				turn=1;
			}
			else if(turn==1)						/* hora de ler a sominha */
			{										/* Lendo o que o outro processo enviou pelo fd2[0] */
				read(fd2[0], &soma, sizeof(soma));	/* Armazenando o resultz que o sugginha mandou */

				printf("Processo %d responde\nO resultado da soma sera: %d\n", i, soma);
				printf("Encerrando processo %d\n", i);
				break;								/* Fim do processo atual */
			}
		}

		/* fechando os arquivinhos restantes */
		close(fd1[1]);
		close(fd2[0]);
	}
	
	/***********************************************************************************************/
	/*.------..------..------..------..------..------..------..------.     .------..------..------.*/
	/*|P.--. ||R.--. ||O.--. ||C.--. ||E.--. ||S.--. ||S.--. ||O.--. |.-.  |P.--. ||A.--. ||I.--. |*/
	/*| :/\: || :(): || :/\: || :/\: || (\/) || :/\: || :/\: || :/\: ((5)) | :/\: || (\/) || (\/) |*/
	/*| (__) || ()() || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |'-.-.| (__) || :\/: || :\/: |*/
	/*| '--'P|| '--'R|| '--'O|| '--'C|| '--'E|| '--'S|| '--'S|| '--'O| ((1)) '--'P|| '--'A|| '--'I|*/
	/*`------'`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'*/
	/***********************************************************************************************/
	
	else
	{
		close(fd1[1]);								/* Neste caso, o processo pai vai ler do primeiro e escrever no segundo */
		close(fd2[0]);								/* Logo, deixa se o fd1[0] aberto para ler e o fd2[1] aberto para escrever */

		int num2[2];
		int soma, j=0;


		while(j<=10)
		{
			if(turn==0)								/* Hora de ler do fd1[0] para fazer a soma */
			{										/* Numeros vindo do pai */
				read(fd1[0], num2, sizeof(num2));	/* Recebeu os numeros de papai */
				turn=1;								/* Muda o turn para o efetuar o passo seguinte */
				j++;
			}else if(num2[0]==0 && num2[1]==0)
				break;
			else if(turn==1)						/* Hora de efetuar a soma e mandar de volta para o suggar daddy */
			{										/* Numeros recebidos de papai */
				soma=num2[0]+num2[1];
				write(fd2[1], &soma, sizeof(soma));	/* Enviando a soma para o papis pelo pipe */
				turn=0;
			}
		}

		close(fd1[0]);								/* Fechando os arquivos para leitura e escrita */
		close(fd2[1]);
	}
}

void pipeDezDez(pid_t pid, int* fd1, int* fd2, int i)
{
	int turn=0;
	
	/***********************************************************************************************/
	/*.------..------..------..------..------..------..------..------.     .------..------..------.*/
	/*|P.--. ||R.--. ||O.--. ||C.--. ||E.--. ||S.--. ||S.--. ||O.--. |.-.  |P.--. ||A.--. ||I.--. |*/
	/*| :/\: || :(): || :/\: || :/\: || (\/) || :/\: || :/\: || :/\: ((5)) | :/\: || (\/) || (\/) |*/
	/*| (__) || ()() || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |'-.-.| (__) || :\/: || :\/: |*/
	/*| '--'P|| '--'R|| '--'O|| '--'C|| '--'E|| '--'S|| '--'S|| '--'O| ((1)) '--'P|| '--'A|| '--'I|*/
	/*`------'`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'*/
	/***********************************************************************************************/
	
	if(i<=0)										/* O pai se comunicara com os 10 filhos */
	{
		
		int num[2];									/* Numeros lidos pelo suggar daddy */
		int soma;									/* Soma recebida do sugginha */
		
		while(1)
		{
			if(turn==0)
			{
				//printf("Processo %d pergunta\nO primeiro numero da soma sera %d\n O segundo numero da soma sera %d\n", i, i, i+1);
				num[0]=i;
				num[1]=i+1;
				
				write(fd1[1], num, sizeof(num));		/* Escrevendo o conteudo no canal 1 */
				turn=1;
			}else if(turn==1)
			{
				read(fd2[0], &soma, sizeof(soma));	/* Armazenando o resultado que o sugginha mandou */

				printf("Processo %d responde\nO resultado da soma sera: %d\nEncerrando processo %d\n", i, soma, i);
				break;								/* Fim do processo atual */
			}
		}
		
	}
	
	/***************************************************************************************************************/
	/*.------..------..------..------..------..------..------..------.     .------..------..------..------..------.*/
	/*|P.--. ||R.--. ||O.--. ||C.--. ||E.--. ||S.--. ||S.--. ||O.--. |.-.  |F.--. ||I.--. ||L.--. ||H.--. ||O.--. |*/
	/*| :/\: || :(): || :/\: || :/\: || (\/) || :/\: || :/\: || :/\: ((5)) | :(): || (\/) || :/\: || :/\: || :/\: |*/
	/*| (__) || ()() || :\/: || :\/: || :\/: || :\/: || :\/: || :\/: |'-.-.| ()() || :\/: || (__) || (__) || :\/: |*/
	/*| '--'P|| '--'R|| '--'O|| '--'C|| '--'E|| '--'S|| '--'S|| '--'O| ((1)) '--'F|| '--'I|| '--'L|| '--'H|| '--'O|*/
	/*`------'`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'`------'`------'*/
	/***************************************************************************************************************/
	
	else
	{
		int num2[2];
		int soma;
		int j=0;
		
		close(fd1[1]);								/* Neste caso, o processo pai vai ler do primeiro e escrever no segundo */
		close(fd2[0]);								/* Logo, deixa se o fd1[0] aberto para ler e o fd2[1] aberto para escrever */
		
		while(j<=10)
		{
			if(turn==0)
			{
				read(fd1[0], num2, sizeof(num2));
				turn=1;
				j++;
			}else if(turn==1)
			{
				soma=num2[0]+num2[1];
				write(fd2[1], &soma, sizeof(soma));
				turn=0;
			}
		}
		
		close(fd1[0]);								/* Fechando os arquivos para leitura e escrita */
		close(fd2[1]);
	}
}
