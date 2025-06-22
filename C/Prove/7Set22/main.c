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
    int fd,fcreato;             /* file descriptor */
    char car;                   /* variabile che conterrà un singolo carattere */
    char* cur;                  /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                    /* Variabile che conterrà il valore di ritorno di fork */
    int status;                 /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;              /* Variabile che conterrà il pid del figlio*/
    int ritorno;                /* Variabile che conterrà il ritrono del filgio*/
    int N;                      /* Numero di figli */
    int n,cnt;                  /* Contatore */
    pipe_t* pipeLine;           /* Canale di comunicazione padre-filgio */




    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    N=argc -1;

    /* allocazione dell'array di N pipe */
    pipeLine = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeLine == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeLine[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }


    /*Creazione del file */
    if ((fcreato=creat("DiCandia", PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", "DiCandia");
       exit(-1);
    }
    
    if ((cur=(char *)malloc(N*sizeof(char))) == NULL)
	{
		printf("Errore allocazione array di caratteri\n");
		exit(4); 
	}

    for(n=0;n<N;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
            for(int k=0;k<N;k++){
                if(n == 0 || k != n - 1){
                    close(pipeLine[k][0]); /* Chiusura dei canali di lettura che non ci interessano */
                }
            
               if(k != n){
                    close(pipeLine[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }

            if((fd=open(argv[n+1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }
            cnt=0;
            while (read(fd,&car,1) != 0){
                
                cnt++;
                if((cnt % 2) == 1){

                    if(n != 0){
                        if((read(pipeLine[n-1][0],cur,N*sizeof(char))) != N){ /* Leggo il contenuto del file */
                           printf("ERRORE NELLA LETTURA 105\n");
                           exit(-1);
                        }
                    }

                    cur[n] = car;
                    if((write(pipeLine[n][1],cur,N*sizeof(char))) != N){ /* Leggo il contenuto del file */
                       printf("ERRORE NELLA SCRITTURA 113 %d\n",n);
                       exit(-1);
                    }

                    ritorno = car;

                }


            }
            
           exit(ritorno);
        }
    }


    for(n=0;n<N;n++){
        close(pipeLine[n][1]);
        if(n != N - 1 ){
            close(pipeLine[n][0]);
        }

    }

    while(read(pipeLine[N-1][0],cur,N*sizeof(char)) > 0){

        if((write(fcreato,cur,N*sizeof(char))) < 0){ /* Leggo il contenuto del file */
           printf("Errore nella lettura 136\n");
           exit(-1);
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
            printf("Filgio %d pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
        }
    }


    exit(0);
}