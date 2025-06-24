#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/stat.h> //SYStem STATus: stat, fstat, S_IFMT, S_IFDIR, S_IFREG
#include <sys/wait.h> //SYStem WAIT: wait
#include <errno.h> //ERror NUmber: errno
#include <sys/types.h> //SYStem TYPES: pid_t
#include <ctype.h> //ISoType: isdigit, isalpha, isalnum
#define PERM 777 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */
typedef char Line[250];

int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd,fdw;                    /* file descriptor */
    Line buffer;               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int nread;                 /* Numero variabili lette da read*/
    int N;                     /* Numero di figli */
    int n,i,j;                     /* Contatore */
    pipe_t* pipeRing;   /* Canale di comunicazione padre-filgio */
    Line *tutteLinee;

    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }

    N= argc -1;

    /*Creazione del file */
    if ((fdw=creat("DICANDIA", PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", "DICANDIA");
       exit(-1);
    }

    /* allocazione dell'array di N pipe */
    pipeRing = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeRing == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeRing[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    tutteLinee = (Line*)malloc(N*sizeof(Line));
    if(tutteLinee == NULL){
        printf("Errore nell allocazione tuttelinee\n");
        exit(-1);
    }


    for (  n = 0;  n < N;  n++){
        
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/

            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               
                if(k!= (n + 1)%N)close(pipeRing[k][1]);

                if(k != n)close(pipeRing[k][0]); 
               
            }



            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }


            


            j=0;
            while ((nread = read(fd,&buffer[j],1)) == 1){
                

                if(buffer[j] == '\n'){

                    if((read(pipeRing[n][0],tutteLinee,N*sizeof(Line))) < 0){ /* Leggo il messaggio del filgio precedente */
                        printf("Errore nella lettura \n");
                        exit(-1);
                    }
                    for(i = 0; i <= j; i++){
                    
                     tutteLinee[n][i] = buffer[i];

                    }

                    if((write(pipeRing[(n+1)%N][1],tutteLinee,N*sizeof(Line))) < 0){ /* Leggo il contenuto del file */
                       printf("Errore nella scrittura \n");
                       exit(-1);
                    }


                    if(n==N-1){

                        for(i=0;i<N;i++){


                           for(j=0; j<250; j++){

                                if((write(fdw,&tutteLinee[i][j],1)) < 0){ /* Leggo il contenuto del file */
                                   printf("Errore nella scrittura 129\n");
                                   exit(-1);
                                }
                                if(tutteLinee[i][j] == '\n'){
                                    break;
                                }

                           }


                        }

                    }

                    ritorno=j;
                    j=0;

                } else j++;

            }
            
           exit(ritorno);
        }

    }


    for(n=1;n<N;n++){
       close(pipeRing[n][0]);
       close(pipeRing[n][1]);
    }
    close(pipeRing[0][0]);

    

    if((write(pipeRing[0][1],tutteLinee,N*sizeof(Line))) < 0){ /* Leggo il contenuto del file */
       printf("Errore nella scrittura 155\n");
       exit(-1);
    }

    close(pipeRing[0][1]);

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
            printf("Il filigo %d con pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
        }


    }

    exit(0);
}