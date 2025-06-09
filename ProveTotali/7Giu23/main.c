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
#define LINESIZE 250 /*Lunghezza buffer*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */


int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char token='S';            /* token sincronizzazione*/
    char linea[LINESIZE];      /* buffer linea*/    
    char max;
    char letto;
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int nread;                 /* Numero variabili lette da read*/
    int N;                     /* Numero di figli */
    int n,i,i_max;                   /* Contatore */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t* pipeFiglioPadre;   /* Canale di comunicazione padre-filgio */



    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(1);
    }

    N=argc-1;

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
            exit(2);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
            /* Chiusura delle pipe inutili */
            for(int k=0;k<N;k++){
                close(pipePadreFiglio[k][1]);
                close(pipeFiglioPadre[k][0]);
               if(k != n){
                    close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                    close(pipePadreFiglio[k][0]); /* Chiusura dei canali di lettura che non ci interessano */
               }
            }

            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }

            ritorno=0;
            i=0;
            while((nread=read(fd,linea + i,1)) == 1){ /* Leggo il contenuto del file */
                
                if(linea[i] == '\n'){
                    ritorno++;
                    printf("DEBUG-FIGLIO scrivo al padre l'ultimo carattere della riga\n");
                    write(pipeFiglioPadre[n][1],linea + i - 1,1);
                    
                    read(pipePadreFiglio[n][0],&token,1);
                    if(token == 'S'){
                        printf("DEBUG-FILGIO %d pid: %d linea: %s\n",n,getpid(),linea);
                    }
                    
                }
                i++;

            }

    


           exit(ritorno);
        }
    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
    
        close(pipePadreFiglio[n][0]);
        close(pipeFiglioPadre[n][1]);

    }

    max = 0;
    for(n=0;n<N;n++){
        read(pipeFiglioPadre[n][0],letto,1);
        if(max < letto){
            max=letto;
            i_max = i;
        }
    }
    
    /* NON SO COME CONTINUARE */




    exit(0);
}