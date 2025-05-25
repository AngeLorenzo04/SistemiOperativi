#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait


int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    char msg[255]; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int L; /*Lunghezza del messaggio*/
    int j; /*Numero di messaggi */
    int piped[2]; /* Canale di comunicazione tra padre e figlio*/


    /*Controllo parametri STRETTO*/
    if (argc != 2){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
        exit(1);
    }

    if(pipe(piped) < 0){
        printf("ERRORE: qualscosa e' andato storto nella creazione della pipe\n");
        exit(2);
    }


    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(3);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/
        
        close(piped[0]);
        /* controllo che il parametro sia un file */
        if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
            printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
            exit(-1);
        }
        L=0;
        j=0;
        
        while (read(fd,msg + L,1)) {
            /* code */

            if(msg[L]=='\n'){

                msg[L]='\0';
                L++;
                write(piped[1],msg,L);
                L=0;
                j++;

            }else L++;

        }
        


        printf("DEBUG-FIGLIO sono stati individuati %d messaggi\n",j);
        exit(j);
    }
    /* codice eseguito solo dal processo padre*/


    close(piped[1]);
    L=0;
    j=0;
  	while (read(piped[0], msg + L, 1)) /* si deve leggere un carattere alla volta per intercettare il carattere '\0'! */
	{ 	
		if (msg[L] == '\0') /* siamo arrivati alla fine di una stringa */
        	{
        		/* dato che il figlio gli ha inviato delle stringhe, il padre le puo' scrivere direttamente con una printf */
        		printf("%d: %s\n", j, msg);
        		j++;	/* incrementiamo il contatore di messaggi */
 			    L=0;  	/* azzeriamo l'indice per la prossima linea */
        	}
        	else L++;	/* se non siamo a fine stringa, incrementiamo l'indice */
	}
	printf("DEBUG-Padre %d letto %d messaggi dalla pipe\n", getpid(), j);


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
        printf("DEBUG-PADRE il processo filgio %d e' terminato con codice %d\n",pidFiglio,ritorno);
    }


    exit(0);
}