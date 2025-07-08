#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait
#define PERM 777 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd,outfile;                    /* file descriptor */
    char buffer[250];
    char token;               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,j,line;                     /* Contatore */
    pipe_t* pipeline;   /* Canale di comunicazione padre-filgio */


    /*Controllo parametri STRETTO*/
    if (argc != 3){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 2 parametri\n");
        exit(-1);
    }

    /* controllo che il parametro sia un valore numerico */
    N = atoi(argv[2]);
    if (N <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
       exit(-1);
    }

    /*Creazione del file */
    if ((outfile=creat("/tmp/RISULTATO", PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", "/tmp/RISULTATO");
       exit(-1);
    }

    /* allocazione dell'array di N pipe */
    pipeline = (pipe_t *)malloc((N-1) * sizeof(pipe_t));
    if (pipeline == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<(N-1);n++){
       if(pipe(pipeline[n]) < 0 ){
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
            for(int k=0;k<N-1;k++){
               if(n == 0 || k != n-1) close(pipeline[k][0]);
               if(k != n) close(pipeline[k][1]);
            }

            /* controllo che il parametro sia un file */
            if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(-1);
            }


            j=0;
            line =  0;
            while (read(fd,&buffer[j],1)){
                
                if(buffer[j] == '\n'){
                    if(line == n){
                        j++;
                        if(n != 0){
                            read(pipeline[n-1][0],&token,1); /*Se non sono al primo giro aspetto l'ok */
                        }

                        write(outfile,buffer,j); /* Ricevuto l'ok scrivo sul file */

                        if(n != N-1){ /* Se non sono l'ultmo figlio*/
                            if((write(pipeline[n][1],&token,1)) < 0){ /* Mando l'ok al figlio successivo */
                               printf("ERRORE: qualcosa è andato storto nella scrittura 100\n");
                               exit(-1);
                            }
                        }
                        

                    }
                    line++;
                    j=0;
                }else j++;

            }
            
           exit(N);
        }

    }

    for(n=0;n<N-1;n++){
        close(pipeline[n][0]);
        close(pipeline[n][1]);
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