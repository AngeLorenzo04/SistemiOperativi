#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char const *argv[])
{
    

/*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char Cx[2];                   /* variabile che conterrà un singolo carattere */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int nread;                 /* Numero variabili lette da read*/
    int N;                     /* Numero di figli */
    int n,num,sum,cnt;                     /* Contatore */
    pipe_t* pipeFiglioPadre;   /* Canale di comunicazione padre-filgio */
    

    /*Controllo parametri LASCO*/
    if (argc < 2){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(-1);
    }


    N=argc-1;

    /* allocazione dell'array di N pipe */
    pipeFiglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeFiglioPadre == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeFiglioPadre[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    for(n=0;n<N;n++){


        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           

            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipeFiglioPadre[k][0]);
               if(k != n){
                    close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }


            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }


            cnt = 0;
            sum = 0;
            while ((nread = read(fd,Cx,2*sizeof(char))) != 0)
            {

               /* controllo che il parametro sia un valore numerico */
               num = atoi(Cx);
               if (num <= 0){
                  printf("ERRORE: parametro non valido %c\nSUGGERIMETNO: inserire un intero positivo\n",Cx[0]);
                  exit(-1);
               }
               sum+=num;
               cnt++;

            }

            if((write(pipeFiglioPadre[n][1],&sum,sizeof(int))) < 0){ /* Leggo il contenuto del file */
               printf("Errore nella lettura ");
               exit(-1);
            }


           exit(cnt);
        }


    }


   /* Chiusura del canale di scrittura di N pipe */
   for(n=0;n<N;n++){
      close(pipeFiglioPadre[n][1]);
   }

   for(n=0;n<N;n++){

        if((read(pipeFiglioPadre[n][0],&num,sizeof(int))) < 0){ /* Leggo il contenuto del file */
           printf("Errore nella lettura ");
           exit(-1);
        }
        printf("Il filgio %d associato al file %s ha raggiunto la cifra %d\n",n,argv[n+1],num);
   }


   for(n=0; n < N ; n++){

        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(-1);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(-1);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("Il filgio %d con pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
        }



   }



    exit(0);
}
