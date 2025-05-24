#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
#include <time.h>


int mia_random(int n){
    srand(time(NULL));
    int casuale;
    casuale = rand() % n;
    return casuale;
}

int main(void){
    
    /*------ VARIABILI USATE ------*/
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/

    pid=getpid();
    printf("pid del processo: %d\n",pid);


    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(1);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/
             
        /* --------------- Variabili Usate Figlio --------------------*/
        pid = getpid();   /* variabile che conterra i vari pid */
        int ppid = getppid(); /* variabile che conterra i vari ppid */
        int random;

        printf("::::::::::: SON ::::::::::::::::\n");
        printf("pid del figlio: %d\n",pid);
        printf("pid del padre: %d\n",ppid);
        random = mia_random(100);
        sleep(10000);
        printf("random: %d\n",random);
        
        exit(random);

    }
    

    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(1);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/
             
        /* --------------- Variabili Usate Figlio --------------------*/
        pid = getpid();   /* variabile che conterra i vari pid */
        int ppid = getppid(); /* variabile che conterra i vari ppid */

        printf("::::::::::: SON ::::::::::::::::\n");
        printf("pid del figlio: %d\n",pid);
        printf("pid del padre: %d\n",ppid);
        
        exit(7);

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
    }

    printf("::::::::::::::PADRE:::::::::::::\n");
    printf("pid del filgio: %d\n",pidFiglio);
    printf("ritorno del filgo: %d\n",ritorno);
    printf("::::::::::::::::::::::::::::::::\n");

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
    }

    printf("::::::::::::::PADRE:::::::::::::\n");
    printf("pid del filgio: %d\n",pidFiglio);
    printf("ritorno del filgo: %d\n",ritorno);
    printf("::::::::::::::::::::::::::::::::\n");


    exit(0);
}
