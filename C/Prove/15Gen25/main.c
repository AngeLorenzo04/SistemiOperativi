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
    int outfile;               /* file descriptor */
    int fd;                    /* file descriptor */
    char linea[250];           /* variabile che conterrà un singolo carattere */
    char token='v';            /* token pipeline */
    char Cx;                   /* Buffer leettura */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,i,nn;                     /* Contatore */
    pipe_t* pipeLine;          /* Canale di comunicazione padre-filgio */



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
    close(fd);


    /* controllo che il parametro sia un valore numerico */
    N = atoi(argv[2]);
    if (N <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
       exit(3);
    }

    /*se il file non eseite lo creo altrimenti lo apro in append */
    if((outfile=open("/tmp/RISULTATO",O_RDWR|O_APPEND|O_CREAT,PERM)) < 0){
       printf(" Errore nella creazione del file\n ");
       exit(4);
    }


    /* allocazione dell'array di N pipe */
    pipeLine = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeLine == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(5);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeLine[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(5);
       }
    }


    for(n=1;n<N+1;n++){
    
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(6);
        }
        if (pid == 0) {
         /* codice eseguito dal processo nipote*/
           

            /* Chiusura del canale di lettura di N pipe */
            for(int k=1;k<N;k++){
               if(k != n - 1 || n == 1) {
                    close(pipeLine[k][0]);
               }
               if(k != n){
                    close(pipeLine[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }


            while (read(pipeLine[n-1][0],&token,1)) {
                fd=open(argv[1],O_RDONLY);
                i=0;
                nn=0;
                while (read(fd,&Cx,1)){
                    if(Cx=='\n'){
                        nn++;
                    }
                    if(nn == n - 1){
                        while (read(fd,linea + i, 1)){
                            printf("DEBUG-%d carattere in linea: %c\n",n,linea[i]);
                            if(linea[i] == '\n'){
                                printf("DEBUG-%d scrivo su file\n",n);
                                write(outfile,linea,i);
                                write(outfile,"\n",1);
                                if(n != N){
                                    write(pipeLine[n][1],&token,1);
                                }
                                break;
                            }else i++;
                        }
                        break;
                    }

                }

                break;
            }
            
            close(fd);
           exit(n);
        }
    
    }


    /* Chiusura dei canali di lettura e scrittura che non servono N pipe */
    for(n=0;n<N;n++){
       close(pipeLine[n][0]);
       if(n != 0){
        close(pipeLine[n][1]);
       }
    }


    /* innesco la pipe line */
    write(pipeLine[0][1],&token,1);

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