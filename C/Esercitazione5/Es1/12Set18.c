#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
#include <ctype.h> //ISoType: isdigit, isalpha, isalnum
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                         /* file descriptor */
    char Cx;                        /* variabile che conterrà un singolo carattere */
    int pid;                        /* Variabile che conterrà il valore di ritorno di fork */
    int status;                     /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;                  /* Variabile che conterrà il pid del figlio*/
    int pidNipote;                  /* Variabile che conterrà il pid del nipote*/
    int ritorno;                    /* Variabile che conterrà il ritrono del filgio*/
    int n;                          /* Variabile per indicizzare i vari file */
    int N;                          /* Numero di file */
    int modFilgio;             /* Modifiche effettuate dal figlio */
    int modNipote;             /* Modifiche effettuate dal figlio */
    long int modTot=0;                /* Modifiche effettuate in tutto */
    pipe_t * pipePadreFiglio;       /* Canale di comunicazione tra il padre e il Figlio*/
    pipe_t * pipePadreNipote;       /* Canale di comunicazione tra il padre e il Nipote*/




    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(1);
    }

    N=argc - 1;


    /* allocazione dell'array di N pipe PADRE-FIGLIO */
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

    /* allocazione dell'array di N pipe  PADRE-NIPOTE*/
    pipePadreNipote = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreNipote == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(4);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipePadreNipote[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(5);
       }
    }

    modFilgio=0;
    for(n=0; n<N; n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(6);
        }
        if (pid == 0) {

            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(pipePadreFiglio[k][0]);
               if(k != n){
                    close(pipePadreFiglio[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }

           modNipote=0; 
           if ((pid = fork()) < 0) { /* creazione con verifica del processo Nipote */
               /* fork fallita */
               printf("Errore in fork\n");
               exit(-1);
           }
           if (pid == 0) {
            /* codice eseguito dal processo Nipote*/

                /* Chiusura del canale di lettura di N pipe */
                close(pipePadreFiglio[n][1]);
                for(int k=0;k<N;k++){
                   close(pipePadreNipote[k][0]);
                   if(k != n){
                        close(pipePadreNipote[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                   }
                }

                /* Apro il file */
                if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
                   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
                   exit(-1);
                }
            
                while (read(fd,&Cx,1) == 1) {   /* Leggo il file carattere per carattere */
                    if(isalpha(Cx) && islower(Cx)){            /* Se trovo un carattere numerico */
                       lseek(fd,-1L,SEEK_CUR);   /* sposto il cursore di uno indietro rispetto a dove mi trovo */
                       Cx=Cx - 32;
                       write(fd,&Cx,1);          /* Lo sostituisco con uno spazio */
                       modNipote++;
                    }
                }

                write(pipePadreNipote[n][1],&modNipote,sizeof(modNipote)); /* Scrivo sulla pipe il valore */
                ritorno=modNipote/256; /* Aggiorno il valore da ritornare secondo la traccia */
                exit(ritorno);
           }

           
           /* controllo che il parametro sia un file */
           if((fd=open(argv[n + 1],O_RDWR)) < 0 ){ //APRO IL FILE
              printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n + 1]);
              exit(-1);
           }

           while (read(fd,&Cx,1) == 1) { /* Leggo il file carattere per carattere */
                if(isdigit(Cx)){ /* Se trovo un carattere numerico */
                   lseek(fd,-1L,SEEK_CUR); /* sposto il cursore di uno indietro rispetto a dove mi trovo */
                   Cx = ' ';
                   write(fd,&Cx,1); /* Lo sostituisco con uno spazio */
                   modFilgio++;
                }
           }
           
           if((pidNipote = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
              printf("ERRORE: wait ha fallito\n");
              exit(-1);
           }
           if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
              printf("Nipote terminato in modo anomalo\n");
               exit(-1);
           }else { //estraggo il valore passato dal figlio
               ritorno = status >> 8;
               /* selezione degli 8 bit piu’ significativi */
               ritorno &= 0xFF;
               printf("DEBUG-FIGLIO nipote: %d ritorno:%d\n", pidNipote,ritorno);
           }
           
           write(pipePadreFiglio[n][1],&modFilgio,sizeof(modFilgio));
           ritorno=modFilgio/256;/* Aggiorno il valore da ritornare secondo la traccia */
           exit(ritorno);
        }
        /* codice eseguito solo dal processo padre*/
    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipePadreFiglio[n][1]);
    }

    for(n=0;n<N;n++){
        read(pipePadreFiglio[n][0],&modFilgio,sizeof(int));
        read(pipePadreNipote[n][0],&modNipote,sizeof(int));
        modTot=modNipote+modFilgio;

        printf("\nDEBUG-PADRE\nnome:%s\nmodFilgio:%d\nmodNipote:%d\nmodTot:%ld\n",argv[n+1],modFilgio,modNipote,modTot);
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
            printf("DEBUG-PADRE il figlio %d e' terminato con codice %d\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}