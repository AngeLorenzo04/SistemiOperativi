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

typedef struct {
    /* data */
    int c1; /*PID nipote*/
    int c2; /*lunghezza della stringa trovata*/
    char c3[250]; /*prima riga del file*/
} data_t;


int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int fd;                     /* file descriptor */
    int pid;                    /* Variabile che conterrà il valore di ritorno di fork */
    int status;                 /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio, pidNipote;   /* Variabile che conterrà il pid del figlio*/
    int ritorno;                /* Variabile che conterrà il ritrono del filgio*/
    int N;                      /* Numero di file*/
    int i;                      /* scorrere i file */
    pipe_t *pipePadreFigli;     /* collegamento tra il padre e i suoi filgi*/
    pipe_t pipeFigloNipote;     /* colegamento tra il Padre(Figlio) e suo figlio(Nipote)*/
    data_t S;                   /* Struct */
    int j;                      /* Contatore */



    /*Controllo parametri LASCO*/
    if (argc < 4){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(1);
    }


    N=argc-1;
    for(i=0; i < N; i++){
        /* controllo che il parametro sia un file */
        if((fd=open(argv[i+1],O_RDWR)) < 0 ){ //APRO IL FILE
           printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[i+1]);
           exit(2);
        }

        close(fd);
    }


    /* allocazione dell'array di N pipe */
    pipePadreFigli = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreFigli == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(i=0;i<N;i++){
       if(pipe(pipePadreFigli[i]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    for(i=0;i<N;i++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(3);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipePadreFigli[k][0]);
               if(k != i){
                    close(pipePadreFigli[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }

            /*Creo canale di comunicazione tra padre e filgio*/
            if(pipe(pipeFigloNipote) < 0){
               printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
               exit(-1);
            }

            if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
                /* codice eseguito dal processo figlio*/
                
                close(pipePadreFigli[i][1]);
                close(pipeFigloNipote[0]);
                
                /* Ridirezione standard output */
                close(1);/*Chiusura STDOUT */
                dup(pipeFigloNipote[1]); /* duplicazione della pipe */
                
                close(pipeFigloNipote[1]);/* chiudo il doppione */

                execlp("sort","sort","-f",argv[i+1],(char*)0);

                /*NEL CASO EXECLP NON VADA A BUON FINE  */
                exit(-1);
            }

            close(pipeFigloNipote[1]); /* chiusura canale di scrittura */
            j=0;
            while (read(pipeFigloNipote[0],S.c3 + j,1) == 1){
                if(S.c3[j] == '\n'){ /* Leggi fino la prima riga */
                    S.c2 = j + 1;
                    break;
                }else j++;
            }

            
            if((pidNipote = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
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
                printf("DEBUG-FIGLIO il processo nipote %d ha ritornato %d\n",pidNipote,ritorno);
                S.c1 = pidNipote;

                write(pipePadreFigli[i][1],&S,sizeof(S));
            }
            

           exit(j-1);
        }


    }

    for(i=0; i < N; i++){
        /* Chiusura del canale di scrittura di N pipe */
        for(i=0;i<N;i++){
           close(pipePadreFigli[i][1]);
        }
    }

    
    for(i=0; i< N ; i++){
        if(read(pipePadreFigli[i][0],&S,sizeof(S)) < 0){
            printf("ERRORE: qualcosa e' andato storto nella read\n");
        }

        printf("DEBUG-PADRE\nc1: %d\nc2: %d\nc3: %s\n\n",S.c1,S.c2,S.c3);

    }


    for ( i = 0; i < N; i++)  {
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
            printf("DEBUG-PADRE il figlio %d ha ritornato %d\n",pidFiglio, ritorno);
        }
    }
    


    exit(0);
}