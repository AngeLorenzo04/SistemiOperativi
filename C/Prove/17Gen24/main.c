#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                                 /* file descriptor */
    char linea[250],car,token,min;          /* variabile che conterrà la linea */
    int pid;                                /* Variabile che conterrà il valore di ritorno di fork */
    int status;                             /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;                          /* Variabile che conterrà il pid del figlio*/
    int ritorno;                            /* Variabile che conterrà il ritrono del filgio*/
    int N;                                  /* Numero di figli */
    int n,i,last_i;                                /* Contatore */
    pipe_t* pipePadreFiglio;                /* Canale di comunicazione padre-filgio */
    pipe_t* pipeFiglioPadre;                /* Canale di comunicazione padre-filgio */




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
          exit(2);
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



    for(n=0;n<N;n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(4);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           

            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipeFiglioPadre[k][0]);
               close(pipePadreFiglio[k][1]);
               if(k != n){
                    close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                    close(pipePadreFiglio[k][0]); /* Chiusura dei canali di lettura che non ci interessano */
               }
            }


            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }

            i=0;

            while ((read(fd,&linea[i],1)) == 1)
            {
                if(linea[i] == '\n'){
                    last_i = i;
                    i=0;
                }else i++;

            }

            linea[last_i + 1] = '\0';
            if((write(pipeFiglioPadre[n][1],&linea[0],1)) < 0 ){

                printf("Errore nella scrittura 105\n");
                exit(-1);

            }

            if((read(pipePadreFiglio[n][0],&token,1)) < 0){

                printf("Errore nella lettura 113\n");
                exit(-1);

            }
            if(token == 'S'){
                printf("\nindice d'ordine: %d pid: %d carattere: %c linea: %s\n",n,getpid(),linea[0],linea);
                ritorno=1;
            }else ritorno = 0;      


           exit(ritorno);
        }



    }



    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipeFiglioPadre[n][1]);
       close(pipePadreFiglio[n][0]);
    }


 
    min=127;

    for(n=0;n<N;n++){
        if((read(pipeFiglioPadre[n][0],&car,1)) != 1){

            printf("Errore nella lettura 140\n");
            exit(5);

        }  

        if( car < min){
            min = car;
            i=n;
        }


    }

    for(n=0;n<N;n++){
        if(n == i){
            token = 'S';

        }else token = 'X';

        if((write(pipePadreFiglio[n][1],&token,1)) < 0 ){

            printf("Errore nella scrittura 168\n");
            exit(-1);

        }
    }


    for(n=0;n<N;n++){

        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(6);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(7);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("Il filgio %d pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
        }


    }



    exit(0);
}