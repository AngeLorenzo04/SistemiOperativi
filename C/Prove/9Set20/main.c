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
    

    /* F L [C1-CQ] */

    /*------ VARIABILI USATE ------*/
    int F;                    /* file descriptor */
    char Cx,Cy,token;                   /* variabile che conterrà un singolo carattere */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int Q,L;                     /* Numero di figli */
    int n,occ;                     /* Contatore */
    pipe_t* pipePF;   /* Canale di comunicazione padre-filgio */
    pipe_t* pipeFP;   /* Canale di comunicazione padre-filgio */


    /*Controllo parametri LASCO*/
    if (argc < 5){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 4 argomenti\n");
        exit(-1);
    }


    Q=argc-3;
    /* controllo che il parametro sia un valore numerico */
    L = atoi(argv[2]);
    if (L <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
       exit(-1);
    }

    /* allocazione dell'array di N pipe */
    pipePF = (pipe_t *)malloc(Q * sizeof(pipe_t));
    if (pipePF == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<Q;n++){
       if(pipe(pipePF[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    /* allocazione dell'array di N pipe */
    pipeFP = (pipe_t *)malloc(Q * sizeof(pipe_t));
    if (pipeFP == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<Q;n++){
       if(pipe(pipeFP[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }



    for(n=0;n<Q;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           


            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<Q;k++){

                close(pipeFP[k][0]);
                close(pipePF[k][1]);

            }


            /* controllo che il parametro sia un file */
            if((F=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(-1);
            }


            occ=0;
            Cy=argv[n+3][0];
            while (read(F,&Cx,1) == 1){
                
                if(Cx == Cy){
                    occ++;
                }

                if(Cx == '\n'){


                    if((read(pipePF[n][0],&token,1)) < 0){ /* Leggo */
                       printf("ERRORE: qualcosa è andato storto nella letttura 115\n");
                       exit(-1);
                    }
                    printf("%d occorrenze del carattere \'%c\'\n",occ,Cy);
                    if((write(pipeFP[n][1],&token,1)) < 0){ /* Scrivo */
                       printf("ERRORE: qualcosa è andato storto nella scrittura 122\n");
                       exit(-1);
                    }
                    ritorno=occ;
                    occ=0;

                }

            }
            


           exit(ritorno);
        }


    }


    for(n=0;n<Q;n++){

        close(pipeFP[n][1]);
        close(pipePF[n][0]);

    }

    for(n=0;n<L;n++){

        printf("Linea %d\n",n+1);
        for(int j = 0; j < Q ; j++){

            if((write(pipePF[j][1],&token,1)) < 0){ /* Scrivo */
               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
               exit(-1);
            }

            if((read(pipeFP[j][0],&token,1)) < 0){ /* Leggo */
               printf("ERRORE: qualcosa è andato storto nella letttura ");
               exit(-1);
            }

        }
        printf("\n");

    }

    for(n=0;n<Q;n++){
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