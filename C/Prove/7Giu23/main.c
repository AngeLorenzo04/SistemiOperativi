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
#define PERM 777 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */


int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char car[2];                  /* variabile che conterrà un singolo carattere */
    char max;
    char token = 's';                /* Variabile che contiene il token */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,i;                     /* Contatore */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t* pipeFIglioPadre;   /* Canale di comunicazione padre-filgio */



    
    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(1);
    }

    N=argc-1;


    /* allocazione dell'array di N pipe */
    pipePadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(2);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipePadreFiglio[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    /* allocazione dell'array di N pipe */
    pipeFIglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeFIglioPadre == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(4);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeFIglioPadre[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(5);
       }
    }


    for(n=0;n<N;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(6);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
           
         /* Chiusura del canale di lettura di N pipe */
         for(int k=0;k<N;k++){
            close(pipeFIglioPadre[k][0]);
            close(pipePadreFiglio[k][1]);
            if(k != n){
                close(pipeFIglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                close(pipePadreFiglio[k][0]);
            }
         }



         /* controllo che il parametro sia un file */
         if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
            printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
            exit(-1);
         }

         i=0;
         while (read(fd,car,2)){

            if(car[1] == '\n' || car[1] == '\0'){
                write(pipeFIglioPadre[n][1],&car,1);
                printf("ATTESA LETTURA FIGLIO %d\n",n);
                while (read(pipePadreFiglio[n][0],&token,1)){
                    printf("DEBUG-FIGLIO %d token: %c\n",n,token);
                    if(token == 'S'){
                        i++;
                        printf("\nDEBUG FILGIO\nprocesso num: %d\nPID: %d\n car: %c\n",n,getpid(),car[0]);
                    }else{
                        ;
                    }
                }
            }

         }
         
         
         exit(i);

        }

    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipeFIglioPadre[n][1]);
       close(pipePadreFiglio[n][0]);
    }



    max=0;
    i=0;
    for(n=0;n<N;n++){

        printf("ATTESA LELTTURA PADRE ...\n");
        read(pipeFIglioPadre[n][0],&car,1);
        printf("valore da analizzare %c massimo %c iterazione: %d\n",car[0],max,n);
        if((int)car[0] > (int)max){
            max=car[0];
            i=n;
        }

    }

    for(n=0;n<N;n++){
        if(n != i){
            token='X';
            write(pipePadreFiglio[n][1],&token,1);
            break;

        }else{
            token = 'S';
            write(pipePadreFiglio[n][1],&token,1);
            break;
        }
    }

    for(n=0;n<N;n++){
        close(pipePadreFiglio[n][1]);
    }

    for(n=0;n<N;n++){

        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(7);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(8);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
        
            printf("DEBUG-PADRE il figlio %d ha ritornato %d\n",pidFiglio,ritorno);

        }


    }




    exit(0);
}