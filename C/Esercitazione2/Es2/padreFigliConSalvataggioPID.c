#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
//O_TRUNC(distrugge contenuto se esiste, O_EXCL(fallisce se il file esiste gia'), O_NDELAY (non bloccante per FIFO e pipe in W)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
#include <time.h>


int mia_random(int n)
{
    srand(time(NULL));
    int casuale;
    casuale = rand() % n;
    casuale++;
    return casuale;
}



int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int *pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int N; /* Variabile che conterrà il numero di fili */
    int n; /* Variabile che indicherà l'indice del filgio corrente */
    int random; /*Variabile che conterrà il valore random*/

  /*Controllo parametri STRETTO*/
  if (argc != 2){
       printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
       exit(1);
  }
    /* controllo che il parametro sia un valore numerico */
    N = atoi(argv[1]);
    if (N <= 0){
        printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[1]);
        exit(1);
    }

    /*Alloco memoria */
    if((pid=calloc(N,sizeof(int))) == NULL){
        printf("ERRORE: qualcosa e' adnato storto nell allocazione  di memoria");
    }
    printf("PADRE-pid : %d\n",getpid());
    printf("PADRE-parametro: %d\n",N);

    for(n=0; n < N; n++){

        if ((pid[n]=fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(2);
        }
        if (pid[n] == 0) {
         /* codice eseguito dal processo figlio*/
            printf(":::::::::: FIGLIO %d ::::::::::::::\n",n+1);
            printf("pid : %d\n",getpid());
            pid[n]=getpid();
            random = mia_random(100 + n);
            printf("random: %d\n",random);
            exit(random);
        }

    }
    /* codice eseguito solo dal processo padre*/
    for(n=0; n<N; n++){
        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(4);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(5);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            for(int j=0 ; j<N; j++){
                if(pidFiglio==pid[j]){
                    printf("::::::::::::::::::::::::::::::::::\n");
                    printf("ritorno del figlio %d indice di creazione %d ha geneerato: %d\n",pidFiglio,j + 1,ritorno);
                }
            }
        }
    }
    exit(0);
}