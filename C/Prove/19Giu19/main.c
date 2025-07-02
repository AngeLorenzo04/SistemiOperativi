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

int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char Cmax,Cx, token='N', linea[250];                   /* variabile che conterrà un singolo carattere */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,stampe,i,i_max,j;                     /* Contatore */
    int *finito;
    pipe_t* pipeFiglioPadre;   /* Canale di comunicazione filgio-padre */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione filgio-padre */
    

    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    N=argc-1;

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


    /* allocazione dell'array di N pipe */
    pipeFiglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeFiglioPadre == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeFiglioPadre[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    finito=(int*)calloc(N,sizeof(int));
    if(finito == NULL){
        printf("errore nel allocazione di memoria per finito\n");
        exit(-1);
    }



    for ( n = 0; n < N; n++) {

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
         
            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipePadreFiglio[k][1]); /* Non serve scrivere sulla pipe PadreFiglio */
               close(pipeFiglioPadre[k][0]); /* Non serve leggere sulla pip Figlio padre*/
               if(k != n){
                    close(pipePadreFiglio[k][0]); /* Chiusura dei canali di lettura PadreFIglio che non ci interessano */
                    close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura FiglioPadre che non ci interessano */
               }
            }


            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }

            i=0;
            j=0;
            stampe=0;
            while (read(fd,&linea[i],1) == 1){
                
                if(linea[i]=='\n'){
                    /* Mando al padre il primo carattere della linea corrente */
                    if((write(pipeFiglioPadre[n][1],&linea[0],1)) < 0){ 
                       printf("ERRORE: qualcosa è andato storto nella scrittura 115\n");
                       exit(-1);
                    }

                    /* Leggo la risposta del padre */
                    if((read(pipePadreFiglio[n][0],&token,1)) < 0){ 
                       printf("ERRORE: qualcosa è andato storto nella letttura 120\n");
                       exit(-1);
                    }
                    if(token == 'S'){ /* Verifico che mi sia dato il via libera per la stampa */
					    // printf("Sono il figlio di indice %d e pid %d e ho trovato una linea con un primo carattere %c con codice ASCII maggiore degli altri nel file %s. Ecco la linea:\n%s", n, getpid(), linea[0], argv[n+1], linea);
                        printf("riga %d massimo %c\n",j,linea[0]);
                        stampe++;
                    }
                    i=0;
                    j++;

                }else i++;


            }

           exit(stampe);
        }

    }
    


    for(n=0;n<N;n++){
        close(pipeFiglioPadre[n][1]);
        close(pipePadreFiglio[n][0]);
    }

    j=0;
    i_max = N+1;
    while (j < N){
        
        Cmax=-1;

        for(n=0;n<N;n++){
            /* IMPORTANTE */
            finito[n] = read(pipeFiglioPadre[n][0],&Cx,1); /* Se il filgio è morto allora non ci sono terminazioni di scrittura aperte per la pipe per cui la READ tornerà 0*/
            if(finito[n]!=1){ /* incremento il numero di processi finiti */
                j++;
            }

            if(finito[n] == 1){
                if(Cx > Cmax){
                    Cmax = Cx;
                    i_max=n;
                }
            }
            

        }
        for(n=0;n<N;n++){

            if(n == i_max){
                token='S';
            }
            if(finito[n] == 1){
                if((write(pipePadreFiglio[n][1],&token,1)) < 0){ /* Scrivo */
                    printf("ERRORE: qualcosa è andato storto nella scrittura 176\n");
                    exit(-1);
                
                }
            }
            token='N';

        }

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