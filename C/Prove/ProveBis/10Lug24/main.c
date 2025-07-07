#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait
#include <time.h>
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */
typedef char line[250];

int mia_random(int n){

    int casuale;
    casuale = rand() % n;
    casuale++;
    return casuale;

}



int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char buffer[250];
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N,L;                     /* Numero di figli */
    int n,i,r,riga;                     /* Contatore */
    pipe_t* pipeline;   /* Canale di comunicazione padre-filgio */
    line* linee;



    /*Controllo parametri LASCO*/
    if (argc < 5){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 4 argomenti\n");
        exit(-1);
    }


    N = (argc-1)/2;

    /* allocazione dell'array di N pipe */
    pipeline = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeline == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeline[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    linee = (line*)malloc(N*sizeof(line));
    if(linee == NULL){
        printf("ERRORE: allocazione andata storta\n");
        exit(-1);
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
                
                if(k != n) close(pipeline[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                if(n == 0 || k != n - 1) close(pipeline[k][0]); /* Chiusura dei canali di lettura */

            }

            /* controllo che il parametro sia un valore numerico */
            L = atoi(argv[2*n+2]);
            if (L <= 0){
               printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2*n+2]);
               exit(-1);
            }

            /* controllo che il parametro sia un file */
            if((fd=open(argv[2*n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[2*n+1]);
               exit(-1);
            }
            


            if(n != 0){
                printf("DEBUG-FIGLIO %d leggo dalla pipe %d...\n",n,n-1);
                if((read(pipeline[n-1][0],linee,N*sizeof(line))) < 0){ /* Leggo */
                   printf("ERRORE: qualcosa è andato storto nella letttura ");
                   exit(-1);
                }
            }
            
            srand(time(NULL));

            r = mia_random(L);


            i=0;
            riga = 0;
            while(read(fd,&buffer[i],1) == 1){

                if(buffer[i] == '\n'){
                    riga++;
                    if(riga == r){
                        i++;
                        buffer[i] = '\0';
                        strcpy(linee[n],buffer);

                        printf("DEBUG-FIGLIO %d scrivo sulla pipe %d la stringa %s..\n",n,n,linee[n]);
                        if((write(pipeline[n][1],linee,N*sizeof(line))) < 0){ /* Scrivo */
                           printf("ERRORE: qualcosa è andato storto nella scrittura \n");
                           exit(-1);
                        }   

                    }
                    i=0;

                }else i++;

            }

           exit(r);
        }

    }



    for(n=0;n<N;n++){
        close(pipeline[n][1]);
        if(n != N-1) close(pipeline[n][0]);

    }


    if((read(pipeline[N-1][0],linee,N*sizeof(line))) < 0){ /* Leggo */
       printf("ERRORE: qualcosa è andato storto nella letttura ");
       exit(-1);
    }

    printf("STAMPO LE LINEE RICEVUTE \n");
    for(n=0;n<N;n++){
        printf("Linea mandata dal filgio %d\n",n);
        printf("    %s\n",linee[n]);
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