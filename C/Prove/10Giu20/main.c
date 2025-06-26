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
    int fd,fdw;                    /* file descriptor */
    char Cx;                   /* variabile che conterrà un singolo carattere */
    char linea[250];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int Q,N;                     /* Numero di figli */
    int q,j,cnt;                     /* Contatore */
    pipe_t* pipeFiglioPadre;   /* Canale di comunicazione padre-filgio */



    /*Controllo parametri LASCO*/
    if (argc < 2){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(-1);
    }
    Q=argc-1;
    
    /*Creazione del file */
    if ((fdw=creat("Camilla", PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", "Camilla");
       exit(-1);
    }


    /* allocazione dell'array di N pipe */
    pipeFiglioPadre = (pipe_t *)malloc(Q * sizeof(pipe_t));
    if (pipeFiglioPadre == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(q=0;q<Q;q++){
       if(pipe(pipeFiglioPadre[q]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    for(q=0; q<Q;q++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<Q;k++){
              close(pipeFiglioPadre[k][0]);
              if(k != q){
                   close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
              }
            }

            /* controllo che il parametro sia un file */
            if((fd=open(argv[q+1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[q+1]);
               exit(-1);
            }

            j=0;
            cnt=0;
            while (read(fd,&linea[j],1) == 1){

                if(linea[j]=='\n'){
                    j++;
                    /* controllo che il parametro sia un valore numerico */
                    N = atoi(&linea[0]);    
                    if( N > 0 && j < 10){
                        cnt++;
                        if((write(pipeFiglioPadre[q][1],linea,j)) < 0){ /* Leggo il contenuto del file */
                           printf("ERRORE: qualcosa è andato storto nella scrittura \n");
                           exit(-1);
                        }

                    }
                    j=0;

                }else j++;
                
            }
            


           exit(cnt);
        }
    }


    /* Chiusura del canale di scrittura di N pipe */
    for(q=0;q<Q;q++){
       close(pipeFiglioPadre[q][1]);
    }

    for( q = 0 ; q < Q ; q++){
        printf("il filgio %d dal file %s ha mandato le seguenti righe\n",q,argv[q+1]);
        j=0;
        while(read(pipeFiglioPadre[q][0],&Cx,1)){ /* Controlla che il figlio non abbia mandato altro*/
            linea[j] = Cx;
            j++;
            while(read(pipeFiglioPadre[q][0],&Cx,1) == 1){ /* Copio il contenuto della pipe in una stringa*/
                linea[j] = Cx;
                j++;
            }
            linea[j] = '\0';
            printf("%s\n",linea);
            if((write(fdw,linea,j)) < 0){ /* Leggo il contenuto del file */
               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
               exit(-1);
            }
            if((write(fdw,"\n",1)) < 0){ /* Leggo il contenuto del file */
               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
               exit(-1);
            }
        }
        printf("\n");
        

        

    }


    for(q=0;q<Q;q++){
        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(-1);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio %d terminato in modo anomalo\n",q);
            exit(-1);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("Il filgio %d ha ritornato %d\n",q,ritorno);
        }
    }

    exit(0);
}