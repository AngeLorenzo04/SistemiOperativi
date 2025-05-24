#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait

int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/


    /*Controllo parametri STRETTO*/
    if (argc != 3){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 2 parametri\n");
        exit(1);
    }

    /* controllo che il parametro sia un file */
    if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
        exit(2);
    }

    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(3);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/

        
        /* ridirezione dello standard output in /dev/null */
        close(1);
        open("/dev/null",O_WRONLY);

        /* ridirezione dello standard err in /dev/null */
        close(2);
        open("/dev/null",O_WRONLY);

        printf("DEBUG-eseguo grep di %s su %s",argv[2],argv[1]);
        execlp("grep","grep",argv[2],argv[1],(char*)0); 

        printf("qualcosa e' andata storto");
        exit(-1);
    }
    /* codice eseguito solo dal processo padre*/

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

        printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi!)\n", pidFiglio, ritorno);
		if (ritorno == 0){ /* verifichiamo il successo del comando grep */
       			printf("Quindi, il figlio %d ha trovato la stringa %s nel file %s\n", pidFiglio, argv[1], argv[2]);
        }else printf("Quindi, il figlio %d NON ha trovato la stringa %s nel file %s oppure il file %s non esiste\n", pidFiglio, argv[1], argv[1], argv[2]);
    }

    exit(0);
}