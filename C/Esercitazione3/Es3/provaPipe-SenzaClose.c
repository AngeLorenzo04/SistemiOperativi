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
    int j; /*Variabile che conterrà il numero di messaggi*/
    int msgsize=5; /*Dimensione dei messaggi*/
    char msg[msgsize]; /*Messaggio*/
    int piped[2]; /*array per la pipe*/

    /*Controllo parametri STRETTO*/
    if (argc != 2){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
        exit(-1);
    }

    /* Creo la pipe */
    if(pipe(piped) < 0){
        printf("Errore nella creaione della pipe\n");
        exit(2);
    }


    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(3);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/

        /* controllo che il parametro sia un file */
        if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
            printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
            exit(1);
        }

        j=0;
        while (read(fd,msg,msgsize) > 0){
            /* code */
            msg[msgsize-1]='\0'; /*sostituiamo il terminatore di riga con il terminatore di strighe*/
            write(piped[1],msg,msgsize); /*Scrivo sulla pipe il messaggio*/
            j++;

        }
        
        close(piped[0]);
        printf("DEBUG-Figlio %d scritto %d messaggi sulla pipe\n", getpid(), j);
        exit(j);
    }
    /* codice eseguito solo dal processo padre*/
    close(piped[1]);
    j=0; /* il padre inizializza la sua variabile j per verificare quanti messaggi ha mandato il figlio */
    while (read(piped[0], msg, msgsize))  /* questo ciclo avra' termine appena il figlio terminera' dato che la read senza piu' scrittore tornera' 0! */
	{ 	
        /* dato che il figlio gli ha inviato delle stringhe, il padre le puo' scrivere direttamente con una printf */
        printf("%d: %s\n", j + 1, msg);
    	j++;	/* incrementiamo il contatore di messaggi */
	}
	printf("DEBUG-Padre %d letto %d messaggi dalla pipe\n", getpid(), j);


    if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
        printf("ERRORE: wait ha fallito\n");
        exit(3);
    }
    if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
       printf("Figlio terminato in modo anomalo\n");
        exit(4);
    }else { //estraggo il valore passato dal figlio
        ritorno = status >> 8;
        /* selezione degli 8 bit piu’ significativi */
        ritorno &= 0xFF;
        printf("il filgio %d ha ritornato %d\n",pidFiglio,ritorno);
    }


    exit(0);
}