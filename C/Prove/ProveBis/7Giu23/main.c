#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
//O_TRUNC(distrugge contenuto se esiste, O_EXCL(fallisce se il file esiste gia'), O_NDELAY (non bloccante per FIFO e pipe in W)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/stat.h> //SYStem STATus: stat, fstat, S_IFMT, S_IFDIR, S_IFREG
#include <sys/wait.h> //SYStem WAIT: wait
#include <errno.h> //ERror NUmber: errno
#include <sys/types.h> //SYStem TYPES: pid_t
#include <ctype.h> //ISoType: isdigit, isalpha, isalnum
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char token,max,Cx;                   /* variabile che conterrà un singolo carattere */
    char buffer[250];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int nread;                 /* Numero variabili lette da read*/
    int N;                     /* Numero di figli */
    int n,j,occ,i_max;                     /* Contatore */
    int* finito;
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t* pipeFiglioPadre;   /* Canale di comunicazione padre-filgio */



    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    N=argc-1;


    finito = (int*)calloc(N,sizeof(int));
    if(finito == NULL){
        printf("ERRORE NEL ALLOCAZIONE DI MEMORIA\n");
        exit(-1);
    }
    memset(finito,N,1);
    /* allocazione dell'array di N pipe */
    pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipePadreFiglio[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


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
               close(pipePadreFiglio[k][1]);
               if(k != n){
                    close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                    close(pipePadreFiglio[k][0]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }


            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }

            j=0;
            occ=0;
            while (read(fd,&buffer[j],1)){

                if(buffer[j] == '\n'){

                    Cx = buffer[j - 1];
                    if((write(pipeFiglioPadre[n][1],&Cx,1)) < 0){ /* Scrivo */
                       printf("ERRORE: qualcosa è andato storto nella scrittura 93\n");
                       exit(-1);
                    }

                    
                    if((read(pipePadreFiglio[n][0],&token,1)) < 0){ /* Leggo */
                       printf("ERRORE: qualcosa è andato storto nella letttura 100\n");
                       exit(-1);
                    }

                    if(token == 'S'){

                        printf("%d stampa\n",n);
                        occ++;
                    }

                    j=0;
                }else j++;

            }
            



           exit(occ);
        }

    }


    for(n=0;n < N;n++){
        close(pipeFiglioPadre[n][1]);
        close(pipePadreFiglio[n][0]);
    }

    while (nread != N){
        nread=0;
        max=-1;
        i_max = N+1;

        for(n=0;n<N;n++){

            finito[n]=read(pipeFiglioPadre[n][0],&Cx,1);
            if(finito[n] != 0){
                if(Cx > max){
                    max = Cx;
                    i_max = n;
                }


            }else nread++;

        }

        for(n=0;n<N;n++){
            
            if(n==i_max){
                token = 'S';
            }
            
            if(finito[n] == 1){
                if((write(pipePadreFiglio[n][1],&token,1)) < 0){ /* Scrivo */
                   printf("ERRORE: qualcosa è andato storto nella scrittura 154\n");
                   exit(-1);
                }
            }
            token='N';

        }

    }

    for(n=0;n<N;n++){
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