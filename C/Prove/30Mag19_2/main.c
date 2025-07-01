#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
//O_TRUNC(distrugge contenuto se esiste, O_EXCL(fallisce se il file esiste gia'), O_NDELAY (non bloccante per FIFO e pipe in W)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/stat.h> //SYStem STATus: stat, fstat, S_IFMT, S_IFDIR, S_IFREG
#include <sys/wait.h> //SYStem WAIT: wait
#include <errno.h> //ERror NUmber: errno
#include <sys/types.h> //SYStem TYPES: pid_t
#include <ctype.h> //ISoType: isdigit, isalpha, isalnum
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */
typedef struct 
{

    int c1; /* PID */
    int c2; /* Lunghezza */
    char c3[250]; /* Linea */

}struct_t;


int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    char buffer[250];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,i;                     /* Contatore */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t pipeFiglioNipote;   /* Canale di comunicazione padre-filgio */
    struct_t S;



    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    N = argc-1;

    /* allocazione dell'array di N pipe */
    pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipePadreFiglio[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    for(n=0;n<N;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipePadreFiglio[k][0]);
               if(k != n){
                    close(pipePadreFiglio[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }            
            }


            /*Creo canale di comunicazione tra padre e filgio*/
            if(pipe(pipeFiglioNipote) < 0){
               printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
               exit(-1);
            }

            if ((pid = fork()) < 0) { /* creazione con verifica del processo nipote */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
             /* codice eseguito dal processo nipote*/
               

                close(pipePadreFiglio[n][1]);
                close(pipeFiglioNipote[0]);
                
                close(1);
                dup(pipeFiglioNipote[1]);
                close(pipeFiglioNipote[1]);

                execlp("sort","sort","-r",argv[n+1],(char*)0);

               exit(-1);
            }



            close(pipeFiglioNipote[1]);
            
            i=0;
            while(read(pipeFiglioNipote[0],&S.c3[i],1)){
                if(S.c3[i] == '\n'){
                    i++;
                    break;
                }else i++;
            }

            S.c2 = i;
            S.c1 = getpid();


            if((write(pipePadreFiglio[n][1],&S,sizeof(S))) < 0){ /* Scrivo */
               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
               exit(-1);
            }

           exit(i);
        }

    }

    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipePadreFiglio[n][1]);
    }

    for(n=0;n<N;n++) {

        printf("\nSTRUTTURA %d associata al file %s\n",n+1,argv[n+1]);

        if((read(pipePadreFiglio[n][0],&S,sizeof(struct_t))) < 0){ /* Leggo */
           printf("ERRORE: qualcosa è andato storto nella letttura ");
           exit(-1);
        }

        printf("  c1:%d\n  c2:%d\n",S.c1,S.c2);
        i=0;
        while (S.c3[i] != 0){
            buffer[i]=S.c3[i];
            i++;
        }
        buffer[i]='\0';
        printf("  c2:%s\n",buffer);
        

    }



    for(n=0;n<N;n++){

        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(-1);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(-1);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("Il filgio %d con pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
        }


    }



    exit(0);
}