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
    int outfile;                  /* file descriptor */
    char buffer[BUFSIZ];                 /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                      /* Variabile che conterrà il valore di ritorno di fork */
    int status;                   /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;                /* Variabile che conterrà il pid del figlio*/
    int ritorno;                  /* Variabile che conterrà il ritrono del filgio*/
    int n;                        /* Variabile per scorrere */
    int N;                        /* Varibile che conterrà il numero */
    pipe_t* pipePadreFigli;       /* Canale di comunicazione padre-filgio */
    pipe_t pipeFigliNipoti;





    /*Controllo parametri STRETTO*/
    if (argc != 3){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 2 parametri\n");
        exit(1);
    }   
    
    /* controllo che il parametro sia un valore numerico */
    N = atoi(argv[1]);
    if (N <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[1]);
       exit(2);
    }

    /*Creazione del file */
    if ((outfile=creat(argv[2], PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", argv[2]);
       exit(-1);
    }



    /* allocazione dell'array di N pipe */
    pipePadreFigli = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipePadreFigli == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipePadreFigli[n]) < 0 ){
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
               close(pipePadreFigli[k][0]);
               if(k != n){
                  close(pipePadreFigli[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }


            /*Creo canale di comunicazione tra Figlio e Nipote*/
            if(pipe(pipeFigliNipoti) < 0){
               printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
               exit(-1);
            }

            if ((pid = fork()) < 0) { /* creazione con verifica del processo Nipote */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
             /* codice eseguito dal processo Nipote*/
               
             /* Chiudo le pipe che non mi servono */
             close(pipeFigliNipoti[0]);
             close(pipePadreFigli[n][1]);


             /* Redirezione dello standard output */
             close(1);
             dup(pipeFigliNipoti[1]);
            
             close(pipeFigliNipoti[1]);/* Chiudo il lato aperto due volte della pipe */
            
             execlp("ps","ps",(char*)0); /* uso execlp  dato che il percorso di ps si trova nel variabile ambiente PATH */

            /* NON SI DOVREBBE MAI TORNARE QUI ! */
             exit(-1);

            }

            close(pipeFigliNipoti[1]);

            /*IL FIGLIO NON FA LA WAIT DEL NIPOTE DATO CHE LAVORANO CONCORRENTEMENTE*/

            sprintf(buffer,"%d",pid); /* salvo nel buffer il pid del nipote */

            /* Redirezione dello standard input */
            close(0);
            dup(pipeFigliNipoti[0]);
            
            /* Redirzione dello standard output */
            close(1);
            dup(pipePadreFigli[n][1]);


            close(pipeFigliNipoti[0]);
            close(pipePadreFigli[n][1]);

            execlp("grep","grep",buffer,(char*)0);

           exit(4);
        }

    }

    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipePadreFigli[n][1]);
    }

    /* Il padre recupera le informazioni dai figli: in ordine di indice */
    for (n=0; n<N; n++)
    {
        int j=0; /* inizializziamo l'indice della linea per la singola linea inviata da ogni figlio */
	/* per ogni figlio si devono leggere le informazioni che il figlio con la grep ha scritto sul suo standard output che corrisponde al lato di scrittura della pipe piped[n]; il padre legge, a questo punto, dal lato di lettura di questa pipe */
        while (read(pipePadreFigli[n][0], buffer + j, 1) == 1)
        {
          	/* dato che arriva una sola linea leggiamo tutti i caratteri dalla pipe */
            j++; /* incrementiamo l'indice della linea */
        }
	/* scriviamo sul file, creato precedentemente, il numero di caratteri corretti e quindi anche con il terminatore di linea! N.B. Il valore di j e' stato incrementato e quindi e' giusto! */
	      write(outfile, buffer, j);	
    }

    /* chiudiamo il file creato dato che comunque il figlio non lo usa */
    close(outfile);


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
          printf("Il filglio %d ha terminato con %d\n",pidFiglio,ritorno);
      }
    }

    exit(0);
}