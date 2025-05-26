#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/stat.h> //SYStem STATus: stat, fstat, S_IFMT, S_IFDIR, S_IFREG
#include <sys/wait.h> //SYStem WAIT: wait



/* versione con 1 pipe e uso di una struttura */
typedef struct 	{
	char c;         /* carattere controllato */
	long int n;     /* numero di occorrenze del carattere */
} tipoS;


int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    char buffer; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int piped[2]; /* Canale di comunicazione padre-filgio */
    int n; /* scorre i caratteri */
    int N; /*Numero di caratteri */
    tipoS msg; /*Messaggio da mandare al padre*/
    

    /*Controllo parametri LASCO*/
    if (argc < 4){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(1);
    }
    
  


    /*Creo canale di comunicazione tra padre e filgio*/
    if(pipe(piped) < 0){
       printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
       exit(3);
    }

    N=argc-2;
    for(n=0; n < N; n++){
        /*creao tanti figli quanti caratteri */

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(4);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/  
            
            /* controllo che il parametro sia un file */
            if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(2);
            }          
            
            msg.c = argv[n+2][0];
            msg.n = 0L;
            
            close(piped[0]);

            while (read(fd,&buffer,1) > 0)  {
                if (buffer == msg.c) {  
                    msg.n++;
                }
                
            }

            write(piped[1],&msg,sizeof(msg));         
            
            exit(msg.c);
        }


    }


    close(piped[1]);
    while (read(piped[0],&msg,sizeof(msg)) > 0) {
        
        printf("DEBUG-PADRE %c: %ld\n",msg.c,msg.n);

    }


    for(n=0;n<N;n++){

        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(5);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(6);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("DEBUG-PADRE il figlio %d ha ritornato %c\n",pidFiglio,ritorno);
        }
    
    }


    exit(0);
}