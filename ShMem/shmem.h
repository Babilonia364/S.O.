#define END 9999

void memUmUm(pid_t pid, int *s, int c, int *mem)
{
	int turn=0;
	
	/*********************************************************/
	/* (                                      (              */
	/* )\ )                                   )\ )           */
	/*(()/( (                (               (()/(    )  (   */
	/* /(_)))(    (    (    ))\ (   (    (    /(_))( /(  )\  */
	/*(_)) (()\   )\   )\  /((_))\  )\   )\  (_))  )(_))((_) */
	/*| _ \ ((_) ((_) ((_)(_)) ((_)((_) ((_) | _ \((_)_  (_) */
	/*|  _/| '_|/ _ \/ _| / -_)(_-<(_-</ _ \ |  _// _` | | | */
	/*|_|  |_|  \___/\__| \___|/__//__/\___/ |_|  \__,_| |_| */
	/*********************************************************/
	
	if(pid>0)
	{
		while(1)
		{
			if(turn==0)
			{
				/* Colocando 2 numeros para o processo ler */
				s=mem;
				printf("Escrevenu na memoria: ");
				for(c=1; c<=2; c++)
				{
					*s++=c;
					printf("%d ", c);
				}
				printf("\nFim da escrita\n");

				*s++=END;
				turn=1;
			}else if(turn==1)
			{
				/* Esperando o processo filho seu terminar */
				wait(NULL);
				s=mem;
				printf("soma: %d\n", *s);
				break;
			}
		}
	}
	
	/*******************************************************************/
	/*(                                      (                         */
	/*)\ )                                   )\ )      (     )         */
	/*(()/( (                (               (()/(  (   )\ ( /(        */
	/*/(_)))(    (    (    ))\ (   (    (    /(_)) )\ ((_))\())  (     */
	/*(_)) (()\   )\   )\  /((_))\  )\   )\  (_))_|((_) _ ((_)\   )\   */
	/*| _ \ ((_) ((_) ((_)(_)) ((_)((_) ((_) | |_   (_)| || |(_) ((_)  */
	/*|  _/| '_|/ _ \/ _| / -_)(_-<(_-</ _ \ | __|  | || || ' \ / _ \  */
	/*|_|  |_|  \___/\__| \___|/__//__/\___/ |_|    |_||_||_||_|\___/  */
	/*******************************************************************/
	
	else
	{
		while(1)
		{
			if(turn==0)
			{
				/* Processo consumidor lendo o que foi colocado na memoria compartilhada */
				printf("Somando o que foi escrito na memoria\n");
				for(s=mem, c=0; *s!=END; s++)
					c=(*s)+c;
				turn=1;
			}else if(turn==1)
			{
				s=mem;
				printf("Escrevendo o conteudo de volta na memoria\n");
				*s=c;
				break;
			}
		}
	}
}
