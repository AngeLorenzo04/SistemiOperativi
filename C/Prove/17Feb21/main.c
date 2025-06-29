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
    char token;                   /* variabile che conterrà un singolo carattere */
    char linea[250];           /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int Q;                     /* Numero di figli */
    int q,occ,i;                     /* Contatore */
    pipe_t* pipeRing;          /* Canale di comunicazione figlio-figlio */


    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    Q=argc-1;


    /* allocazione dell'array di N pipe */
    pipeRing = (pipe_t *)malloc(Q * sizeof(pipe_t));
    if (pipeRing == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(q=0;q<Q;q++){
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
               if(k!=q) close(pipeRing[k][0]);
               if(k!=(q+1)%Q) close(pipeRing[k][1]);
            }


            /* controllo che il parametro sia un file */
            if((fd=open(argv[q+1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[q+1]);
               exit(-1);
            }


            occ=0;
            i=0;
            while (read(fd,&linea[i],1) == 1){
                

                if(isdigit(linea[i])) occ++;

                if(linea[i] == '\n'){
                    linea[i] = '\0';
                    if((read(pipeRing[q][0],&token,1)) < 0){ /* Leggo */
                       printf("ERRORE: qualcosa è andato storto nella letttura 91\n");
                       exit(-1);
                    }

                    printf("Figlio con indice %d e pid %d ha letto %d caratteri numerici nella linea %s\n",q,getpid(),occ,linea);

                    if((write(pipeRing[(q+1)%Q][1],&token,1)) < 0){ /* Scrivo */
                       printf("ERRORE: qualcosa è andato storto nella scrittura 98\n");
                       exit(-1);
                    }

                    i=0;
                    ritorno=occ;
                    occ=0;
                }else i++;

            }
            

           exit(ritorno);
        }


    }

    for(q=0;q<Q;q++){

        close(pipeRing[q][0]);
        if(q!=0) close(pipeRing[q][1]);

    }


    if((write(pipeRing[0][1],&token,1)) < 0){ /* Scrivo */
       printf("ERRORE: qualcosa è andato storto nella scrittura 123\n");
       exit(-1);
    }

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
            printf("Il filgio %d con pid %d ha ritornato %d\n",q,pidFiglio,ritorno);
        }
    }


    exit(0);
}