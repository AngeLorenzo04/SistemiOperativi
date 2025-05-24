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

int main(int argc, char* argv[]) {


    /*------ VARIABILI USATE ------*/
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int N; /* numero di figli */
    int n; /* indice per i figli */

      /*Controllo parametri STRETTO*/
    if (argc != 2){
         printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
         exit(1);
    }
    
    /* controllo che il parametro sia un valore numerico */
    N = atoi(argv[1]);
    if (N <= 0){
        printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[1]);
        exit(2);
    }

    /* creazione figli */
    for (n=0; n < N; n++)
    {
        if ((pid=fork()) < 0){
            printf("ERRORE: fork non andata a buon fine per il figlio %d \n", n);
            exit(1);
        }
        if (pid==0){ 
            /* codice figlio */
            
            printf("::::::::::::::::::: FILGIO %d ::::::::::::::::\n",n + 1);
            printf("Figlio numero: %d\n",n + 1);
            printf("pid corrente: %d\n",getpid());

            exit(n + 1);
        }
    } /* fine for */


    for(n=0; n <N; n++){
        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(3);
        }
        if(n==0){
            printf("::::::::::::::::: PADRE ::::::::::::::::\n");
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio %d terminato in modo anomalo\n",pidFiglio);
            exit(4);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("ritorno del figlio %d: %d\n",pidFiglio,ritorno);
        }
        

    }
    printf("pid processo padre: %d\n",pid);
    printf("parametro passato: %d\n",N);
    exit(0);
}