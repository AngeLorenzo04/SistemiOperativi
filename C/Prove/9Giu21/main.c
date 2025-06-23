#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
#define PERM 777 /*Permessi da dare al file creato*/
#define LSIZE 200
typedef int pipe_t[2]; /* Tipo per creare più pipe */


int main(int argc, char* argv[]) {
    

    // /*------ VARIABILI USATE ------*/
    int fd,fcreato;                    /* file descriptor */
    char buffer[200];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N,X;                     /* Numero di figli */
    int n,j,i;                     /* Contatore */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t pipeSpeciale;        


    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }

    N=argc-1;


    /*Creazione del file */
    if ((fcreato=creat("ANGELORENZO", PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", "ANGELORENZO");
       exit(-1);
    }

    /*Creo canale di comunicazione tra padre e filgio*/
    if(pipe(pipeSpeciale) < 0){
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

     /* Chiusura del canale di lettura di N pipe */
     close(pipeSpeciale[0]);
     
     close(0);
     open(argv[1],O_RDONLY);
    
     close(1);
     dup(pipeSpeciale[1]);
     close(pipeSpeciale[1]);

     execlp("wc","wc","-l",(char*)0);

     exit(-1);

    }


    close(pipeSpeciale[1]);

    n=0;
    while(read(pipeSpeciale[0],buffer + n,1) == 1){

        n++;

    }

    if(n!=0){

        /* controllo che il parametro sia un valore numerico */
        buffer[n-1]='\0';
        X = atoi(buffer);
        if (X <= 0){
           printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",buffer);
           exit(-1);
        }


    }else printf("ERRORE NELLA LETTURA DELLA PIPE\n");    

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
        printf("Il figlio speciale ha ritornato %d\n",ritorno);
    }


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

    for(n=0; n < N ; n++){

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


            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }
            
            j=0;
            while(read(fd,buffer + j,1) != 0){
                
                if(buffer[j] == '\n'){

                    j++;
                    buffer[j + 1] = '\n';
                    if((write(pipePadreFiglio[n][1],&j,sizeof(j))) < 0){ /* Leggo il contenuto del file */
                       printf("Errore nella lettura 166\n");
                       exit(-1);
                    }
                    if((write(pipePadreFiglio[n][1],buffer,j)) < 0){ /* Leggo il contenuto del file */
                       printf("Errore nella lettura 170\n");
                       exit(-1);
                    }
                    ritorno=j;
                    j=0;

                }else j++;

            }


            exit(ritorno);
        }

    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipePadreFiglio[n][1]);
    }

    for(i=0; i < X; i++){

        for(n=0;n<N;n++){

            if((read(pipePadreFiglio[n][0],&j,sizeof(j))) < 0){ /* Leggo il contenuto del file */
               printf("Errore nella lettura ");
               exit(-1);
            }
            printf("La riga %d del file %d ha lunghezza %d\n",i,n,j);
            if((read(pipePadreFiglio[n][0],buffer,j)) < 0){ /* Leggo il contenuto del file */
               printf("Errore nella lettura ");
               exit(-1);
            }
            if((write(fcreato,buffer,j)) < 0){ /* Leggo il contenuto del file */
               printf("Errore nella lettura 205\n");
               exit(-1);
            }       

        }
        printf("\n");

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
            printf("Il figlio %d con pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
        }

    }

    exit(0);
}