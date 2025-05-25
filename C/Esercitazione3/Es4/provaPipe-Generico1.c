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
    int piped[2]; /* Canale di comunicazione Padre-Figlio */
    int L; /*Lunghezza singolo messaggio*/
    int j; /*Numero messaggi*/

    /*Controllo parametri STRETTO*/
    if (argc != 2){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
        exit(1);
    }

    
    if((pipe(piped)) < 0){
        printf("ERRORE: qualcosa e' andato storto nella creazione della pipe :(\n");
        exit(2);
    }



    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(3);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/
        
        close(piped[0]); /*chiudo la terminazione di lettura della pipe*/

        /* controllo che il parametro sia un file */
        if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
            printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[2]);
            exit(-1);
        }

        L=0;
        j=0;
        while (read(fd,msg + L,1) != 0){
           
            if(msg[L] == '\n'){
                
                msg[L]='\0';
                L++;
                printf("DEBUG-FIGLIO msg:%s L:%d\n",msg,L);
                write(piped[1],&L,sizeof(L)); /*Passo al padre la dimensione del messaggio*/
                write(piped[1],msg,L); /*Passo al padre il messaggio*/
                L=0;
                j++;
            }
            else L++;
        }           


		printf("DEBUG-FIGLIO %d scritto %d messaggi sulla pipe\n", getpid(), j);
        exit(j);
    }
    /* codice eseguito solo dal processo padre*/
    close(piped[1]); /*chiudo la terminazione di scrittura della pipe*/

    j=0;
    while (read(piped[0],&L,sizeof(L))) { /*Finchè nella pipe sono presenti lunghezze di file*/
        
        read(piped[0],msg,L); /*Leggo il messaggio*/
        printf("%d:%s\n",j+1,msg);
        j++;

    }
    printf("DEBUG-PADRE il processo %d ha ricevuto %d messaggi (^-^) \n",getpid(),j);

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

        printf("DEBUG-PADRE il processo filgio %d e' terminato con il codice %d\n",pidFiglio,ritorno);
    }
    



    exit(0);
}