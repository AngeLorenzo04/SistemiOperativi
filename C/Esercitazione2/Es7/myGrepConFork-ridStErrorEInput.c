#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
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

    if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
        /* fork fallita */
        printf("Errore in fork\n");
        exit(2);
    }
    if (pid == 0) {
     /* codice eseguito dal processo figlio*/
        
        /*Ridirezione dello standard input */
        close(0);
        if((fd=open(argv[2],O_RDWR)) < 0 ){ //APRO IL FILE
            printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[2]);
            exit(2);
        }

        /*Ridirezione dello standard output*/
        close(1);
        open("/dev/null",O_WRONLY);

        /*Ridirezione dello standard err*/
        close(2);
        open("/dev/null",O_WRONLY);

        /*eseguo con execlp il comando filtro grep*/
        execlp("grep","grep",argv[1]);

        printf("ERRORE: qualcosa e' andato storto durante il grep :(\n");
        exit(-1);
    }
    /* codice eseguito solo dal processo padre*/

    if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
        printf("ERRORE: wait ha fallito\n");
        exit(3);
    }
    if ((status & 0xFF) != 0){ //verificare che lo status sia valido
       printf("Figlio terminato in modo anomalo\n");
        exit(4);
    }else { //estraggo il valore passato dal figlio
        ritorno = status >> 8;
        /* selezione degli 8 bit piu’ significativi */
        ritorno &= 0xFF;

        if(ritorno != 0){
            printf("La stringa %s non e' presente nel file %s\n",argv[1],argv[2]);
        }else printf("La stringa %s e' presente nel file %s\n",argv[1],argv[2]);
    }
    exit(0);
}