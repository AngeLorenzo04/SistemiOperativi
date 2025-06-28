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
#define LSIZE 250
typedef int pipe_t[2]; /* Tipo per creare più pipe */
typedef struct 
{

    int c1;     /* PID */
    char c2;    /* Secondo carattere */
    char c3;    /* Penultimo carattere */

}stru_t;


int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int F;                    /* file descriptor */
    char linea[LSIZE];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int nriga;                 /* Numero variabili lette da read*/
    int L;                     /* Numero di figli */
    int n,i;                     /* Contatore */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    stru_t s;

    
    /*Controllo parametri STRETTO*/
    if (argc != 3){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 2 parametri\n");
        exit(-1);
    }
    

    /* controllo che il parametro sia un valore numerico */
    L = atoi(argv[2]);
    if (L <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
       exit(-1);
    }

    /* allocazione dell'array di N pipe */
    pipePadreFiglio = (pipe_t *)malloc(L * sizeof(pipe_t));
    if (pipePadreFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<L;n++){
       if(pipe(pipePadreFiglio[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }



    for(n=0; n<L;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           

            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<L;k++){
               close(pipePadreFiglio[k][0]);
               if(k != n){
                    close(pipePadreFiglio[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }



            /* controllo che il parametro sia un file */
            if((F=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(-1);
            }


            i=0;
            nriga=0;
            while (read(F,&linea[i],1) == 1)
            {

                if(linea[i]=='\n'){
                    
                    nriga++;

                    if(nriga == n+1 ){
                        s.c1 = getpid();
                        s.c3 = linea[i-2];
                        s.c2 = linea[1];
                        if((write(pipePadreFiglio[n][1],&s,sizeof(s))) < 0){ /* Scrivo il contenuto del file */
                           printf("ERRORE: qualcosa è andato storto nella scrittura 115\n");
                           exit(-1);
                        }
                    }
                    i=0;
                }else i++;
                
            }
            


           exit(n + 1);
        }


    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<L;n++){
       close(pipePadreFiglio[n][1]);
    }

    for(n=0;n<L;n++){

        if((read(pipePadreFiglio[n][0],&s,sizeof(s))) < 0){ /* Leggo */
           printf("ERRORE: qualcosa è andato storto nella letttura 141\n");
           exit(-1);
        }

        if(s.c2 == s.c3){
            printf("Il filgio %d associato alla riga %d ha mandato la seguente truttura\n     c1:%d\n     c2:%c\n     c3:%c\n",n,n+1,s.c1,s.c2,s.c3);
            printf("\n");

        }

    }


    for(n=0;n<L;n++){

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