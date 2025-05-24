#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <sys/wait.h> //SYStem WAIT: wait
#include <unistd.h>

int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/

    /*Controllo parametri STRETTO*/
    if (argc != 2){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
        exit(1);
    }

    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(2);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/
        printf("DEBUG-Esecuzione di ls -l da parte del figlio con pid %d e con parametro %s\n", getpid(), argv[1]);
        execlp("ls","ls","-l",argv[1],(char*)0); // nei processi figli si usa sempre la versione con la p
        // in questo caso passiamo il comando "ls -l $parametro" e come ultimo parametro passiamo un char pointer a 0 (zero terminatore)

        printf("Errore in execlp\n");
		exit(-1); /* torniamo al padre un -1 che sara' interpretato come 255 e quindi identificato come errore */
    }
    
    /* codice eseguito solo dal processo padre*/
    if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
        printf("ERRORE: wait ha fallito\n");
        exit(2);
    }
    if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
       printf("Figlio terminato in modo anomalo\n");
        exit(3);
    }else { //estraggo il valore passato dal figlio
        ritorno = status >> 8;
        /* selezione degli 8 bit piu’ significativi */
        ritorno &= 0xFF;
        printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
    }

    exit(0);
}