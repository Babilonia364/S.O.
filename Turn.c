#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER 256

int main()
{
    int fd1[2], fd2[2];    /* Canais de escrita e leitura */
    int turn=0; /* Variável que define o que cada processinho vai fazer */
    pid_t pid;    /* Pidão dos processões da massa */

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

    /* Criando o processao da massa */
    if((pid=fork())<0)
    {
        perror("fork");
        exit(1);
    }

    /* Criando o processo pagador de contas */
    if(pid>0)
    {
        /* No process sugarDaddy, vamos escrever, então fechar a leitura desse lado */
        close(fd1[0]);
        close(fd2[1]); /* fecha - se um canal para escrever e ler em cada documento */

        int num[2]; /* Numeros lidos pelo suggar daddy */
        int soma; /* Soma recebida do sugginha */

        while(1)
        {
            if(turn==0) /* Sugar Daddy vai escrever para o suggar kiddo */
            {
                printf("Digite o primeiro numero da soma: ");
                scanf("%d", &num[0]);
                printf("\nDigite o segundo numero da soma: ");
                scanf("%d", &num[1]);
                printf("\n");

                write(fd1[1], num, sizeof(num));
                turn=1;
            }else if(turn==1) /* hora de ler a sominha */
            {
                read(fd2[0], &soma, sizeof(soma)); /* Armazenando o resultz que o sugginha mandou */

                printf("O resultz da soma sera: %d\n", soma);
                turn=0; /* Hora de escrever dinovu 2 numeritos */
            }
        }

        /* fechando os arquivinhos restantes */
        close(fd1[1]);
        close(fd2[0]);
    }else
    {
        close(fd1[1]); /* O processo kiddo vai ler do primeiro e escrever no segundo */
        close(fd2[0]); /* Logo, deixa se o fd1[0] aberto para ler e o fd2[1] aberto para escrever */

        int num2[2];
        int soma;


        while(1)
        {
            if(turn==0)                /* Hora de ler do fd1[0] para fazer a soma */
            {                                    /* Numeros vindo do pai */
                read(fd1[0], num2, sizeof(num2)); /* Recebeu os numeros de papai */
                turn=1;                 /* Muda o turn para o efetuar o passo seguinte */
            }else if(turn==1) /* Hora de efetuar a soma e mandar de volta para o suggar daddy */
            {                                    /* Numeros recebidos de papai */
                soma=num2[0]+num2[1];

                write(fd2[1], &soma, sizeof(soma)); /* Enviando a soma para o papis pelo pipe */
                turn=0;
            }
        }

        close(fd1[0]);            /* Fechando os arquivos para leitura e escrita */
        close(fd2[1]);
    }

    return(0);
}

