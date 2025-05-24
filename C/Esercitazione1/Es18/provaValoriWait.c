#include <stdio.h> //Standard I/O: printf, BUFSIZ
//O_TRUNC(distrugge contenuto se esiste, O_EXCL(fallisce se il file esiste gia'), O_NDELAY (non bloccante per FIFO e pipe in W)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait

int main(void) { //void perchè non vengono passati parametri
    
    /*------ VARIABILI USATE ------*/
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int valore; /* Variabile che conterrà il valore inserito dal utente*/

    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(1);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/

        printf("::::::::::::: FIGLIO ::::::::::::\n");
         
        printf("INSERIRE: valore numerico compreso tra 1 e 254: ");
        scanf("%d", &valore);
        if(valore <= 0 || valore >= 255){
            printf("ATTENZIONE: il valore verrà troncato :( \n");
        }else printf("CHILL: il valore non verrà troncato :) \n");

        printf("\n");

        exit(valore);
    }
    /* codice eseguito solo dal processo padre*/


    if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
        printf("ERRORE: wait ha fallito\n");
        exit(1);
    }
    if ((status & 0xFF) != 0){ //verificare che lo status sia valido
       printf("Figlio terminato in modo anomalo\n");
        exit(2);
    }else { //estraggo il valore passato dal figlio
        ritorno = status >> 8;
        /* selezione degli 8 bit piu’ significativi */
        ritorno &= 0xFF;
    }

    printf(":::::::::::::::: PADRE ::::::::::::::::\n");
    printf("PID del filgio: %d\n",pidFiglio);
    printf("Valore inserito: %d\n",ritorno);
    printf("\n");


    
    exit(0);
}