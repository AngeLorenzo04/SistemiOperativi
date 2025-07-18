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
    int F;                      /* file descriptor */
    char Cx,token;                    /* variabile che conterrà un singolo carattere */
    int L;                      /* Numero di righe */
    int pid;                    /* Variabile che conterrà il valore di ritorno di fork */
    int status;                 /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;              /* Variabile che conterrà il pid del figlio*/
    int ritorno;                /* Variabile che conterrà il ritrono del filgio*/
    int Q;                      /* Numero di figli */
    int q,cnt;                      /* Contatore */
    pipe_t* pipeRing;           /* Canale di comunicazione padre-filgio */


    /*Controllo parametri LASCO*/
    if (argc < 5){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 4 argomenti\n");
        exit(-1);
    }

    /* controllo che il parametro sia un valore numerico */
    L = atoi(argv[2]);
    if (L  == 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
       exit(-1);
    }

    Q=argc-3;

    /* allocazione dell'array di N pipe */
    pipeRing = (pipe_t *)malloc(Q * sizeof(pipe_t));
    if (pipeRing == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(q=0;q<Q + 1;q++){
       if(pipe(pipeRing[q]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    for(q=0;q<Q;q++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           /* Chiusura del canale di lettura di N pipe */
           for(int k=0;k<Q;k++){
                if(k!=q){

                    close(pipeRing[k][0]);

                }
                if(k !=  q + 1){
                    close(pipeRing[k][1]);
                }
           }

           /* controllo che il parametro sia un file */
           if((F=open(argv[1],O_RDONLY)) < 0 ){ //APRO IL FILE
              printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
              exit(-1);
           }

           cnt=0;
           while (read(F,&Cx,1) > 0){  
          
                if(Cx == '\n'){
                    if((read(pipeRing[q][0],&token,1))<0){ /* Aspetto il via */
                        printf("Errore nella lettura 93\n");
                        exit(-1);
                    }

                	printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, getpid(), cnt, argv[q+3][0]);

                    if((write(pipeRing[q+1][1],&token,1))<0){ /* Do il via al figlio successivo */
                       printf("Errore nella lettura 100\n");
                       exit(-1);
                    }

                    ritorno = cnt;
                    cnt = 0;

                }else  if(Cx == argv[q+3][0]){
                    cnt++;
                }
          
            }


           exit(cnt);
        }    


    }

    for(q=0;q<Q + 1;q++){
         if(q != Q){
             close(pipeRing[q][0]);
         }
         if(q != 0){
             close(pipeRing[q][1]);
         }
     }

    token='v';
    for(int i = 0; i<L; i++){
        printf("\nAnalizzo la riga %d\n",i);
        if((write(pipeRing[0][1],&token,1)) < 0){ /* Scrivo il toke e do il via */
           printf("Errore nella Scrittura 135\n");
           exit(-1);
        }
        if((read(pipeRing[Q][0],&token,1)) < 0){ /* Leggo il token e vado avanti */
           printf("Errore nella lettura 140 \n");
           exit(-1);
        }
    }

    printf("\n");

    for(q=0;q<Q;q++){
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
            printf("Il figlio %d ha ritornato %d\n",q,ritorno);
        }
    }

    exit(0);
}