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
    int c1; /* PID */
    long int c2; /* OCCORRENZE*/
} struct_t;




int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char Cx,Cy;                   /* variabile che conterrà un singolo carattere */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n;                     /* Contatore */
    long int occ;
    pipe_t* pipeLine;   /* Canale di comunicazione padre-filgio */
    struct_t* S;



    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    N=argc-2;

    /* allocazione dell'array di N pipe */
    pipeLine = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeLine == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeLine[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    S=(struct_t*)malloc(N*sizeof(struct_t));
    if(S == NULL){
        printf("Errore nell allocazione array dinamico per la struttura\n");
        exit(-1);
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
               if(n == 0 || k != n - 1) close(pipeLine[k][0]);
               if(k != n) close(pipeLine[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
            }

            /* controllo che il parametro sia un file */
            if((fd=open(argv[1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(-1);
            }

            Cy = argv[n+2][0];

            occ=0;
            while(read(fd,&Cx,1) == 1){

                if(Cx == Cy) occ++;

            }

            if(n != 0){
                if(read(pipeLine[n-1][0],S,N*sizeof(struct_t)) < 0){
                    printf("ERRORE: qualcosa è andato male nella lettura\n");
                    exit(-1);
                }
            }

            S[n].c1=getpid();
            S[n].c2 = occ;

            if((write(pipeLine[n][1],S,N*sizeof(struct_t))) < 0){ /* Scrivo */
               printf("ERRORE: qualcosa è andato storto nella scrittura 117\n");
               exit(-1);
            }


           exit(occ);
        }

    }

    for(n=0;n<N;n++){

        close(pipeLine[n][1]);
        if(n != N-1) close(pipeLine[n][0]);

    }

    if((read(pipeLine[N-1][0],S,N*sizeof(struct_t))) < 0){ /* Leggo */
       printf("ERRORE: qualcosa è andato storto nella letttura \n");
       exit(-1);
    }


    printf("Stampo le strutture dati\n");
    for(n=0; n < N; n++){

        printf("    c1:%d c2:%ld\n",S[n].c1,S[n].c2);

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