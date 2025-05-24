#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait

int main(int argc, char* argv[]) {
   
    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    char Cx; /* variabile che conterrà un singolo carattere */
    char buffer; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int N; /* numero di file passati */
    int n; /* indice file che si sta studiando */
    int totale; /* numero di volte che il carattere si presenta nel file*/
    int nread; /* caratteri letti per volta dalla read */

    /*Controllo parametri LASCO*/
    if (argc < 4){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire almeno 3 parametri\n");
        exit(1);
    }

    /* controllo che il parametro sia una carattere */
    if(strlen(argv[argc - 1]) != 1){
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire un singolo carattere\n",argv[argc - 1]);
        exit(2);
    }
    Cx=argv[argc -1][0];
    N=argc-2;

    printf("DEBUG-PADRE Cx: %c\n",Cx);
    for(n=0; n < N; n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(3);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/

         /* controllo che il parametro sia un file */
            printf(":::::::::::: FIGLIO %d :::::::::::\n",n + 1);
            printf("pid:%d\n",getpid());
            printf("\n");
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
                printf("ERRORE-FIGLIO: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
                exit(-1);
            }
            
            while((nread=read(fd,&buffer,1)) == 1){
                if(buffer == Cx){
                    totale++;
                }
            }

            exit(totale);
        }        
    }

    for(n=0; n<N; n++){
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
            if(ritorno != 255){
                printf(":::::::: FIGLIO %d ::::::::::\n",pidFiglio);
                printf("ritorno filgio: %d\n",ritorno);
                printf("\n");
            }else printf("ERRORE: qualcos e' andato storto :( con il figlio %d\n",pidFiglio);
        }       
    }

    exit(0);
}