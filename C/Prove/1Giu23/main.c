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


typedef struct {
    
    int c1;         /* Deve contenere il pid del nipote */
    char c2[250];   /* Deve contenere l'ultima riga del file */
    int c3;         /* Deve contenere la lunghezza del' ultima riga */

}stru;


int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    char buffer[255];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int fd;                    /* file descriptor */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,i;                   /* Contatore */
    stru* S;                    /* Struct */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t pipeFiglioNipote;   /* Canale di comunicazione padre-filgio */



    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(1);
    }

    N=argc-1;

    for(n=0;n<N;n++){

        /* controllo che il parametro sia un file */
        if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
           printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
           exit(2);
        }   
        close(fd);

    }

    /* allocazione dell'array di struct */
    S = (stru*)malloc(N*sizeof(stru));
    if(S == NULL){
      printf("Errore nell allocazione array dinamico per le S\n");
      exit(3);
    }

    /* allocazione dell'array di N pipe */
    pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(4);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipePadreFiglio[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(4);
       }
    }


    for(n=0;n<N;n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(5);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           


            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipePadreFiglio[k][0]);
               if(k != n){
                    close(pipePadreFiglio[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }


            fd = open(argv[n+1],O_RDONLY);

            /*Creo canale di comunicazione tra padre e filgio*/
            if(pipe(pipeFiglioNipote) < 0){
               printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
               exit(-1);
            }

            if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
             /* codice eseguito dal processo figlio*/
               
                close(pipePadreFiglio[n][1]);
                close(pipeFiglioNipote[0]);

                /*Ridirezione dello standard output */
                close(1);
                dup(pipeFiglioNipote[1]);
                 
                close(pipeFiglioNipote[1]);


                /* Il nipote diventa il comando rev */

                execlp("rev","rev",argv[n+1],(char*)0);


             /* QUI NON SI DEVE RITORNARE*/
               exit(-1);
            }

            close(pipeFiglioNipote[1]);

           while (read(pipeFiglioNipote[0],buffer + i,1)){

                if(buffer[i] == '\n'){
                    i++;
                    S[n].c3 = i;
                    i=0;
                }else i++;
           }            

           for(int j = 0; j < i; j++){
               S[n].c2[j] = buffer[j];
           }
           write(pipePadreFiglio[n][1],S + n,sizeof(stru));
            exit(i);
        }
    }
    
    
    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipePadreFiglio[n][1]);
    }


    for(n=0; n < N; n++){
      read(pipePadreFiglio[n][0],S + n,sizeof(stru));
      printf("DEBUG-PADRE struct %d\nc1: %d\nc2: %s\nc3: %d\n",n+1,S[n].c1,S[n].c2,S[n].c3);
    }

    for(n=0;n<N;n++){

      if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
          printf("ERRORE: wait ha fallito\n");
          exit(8);
      }
      if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
         printf("Figlio terminato in modo anomalo\n");
          exit(9);
      }else { //estraggo il valore passato dal figlio
         ritorno = status >> 8;
         /* selezione degli 8 bit piu’ significativi */
         ritorno &= 0xFF;
         printf("DEBUG-PADRE il filgio %d ha ritornato %d\n",pidFiglio,ritorno);
      
      }


    }


    exit(0);

}