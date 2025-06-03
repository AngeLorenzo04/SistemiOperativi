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
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n;                     /* Contatore */
    int outfile;               /* file descriptor */
    char linea[255];
    int i;
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
    pipe_t pipeFiglioNipote;   /* Canale di comunicazione padre-filgio */



    /*Controllo parametri LASCO*/
    if (argc < 4){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(1);
    }

    N=argc-2;

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
          exit(3);
       }
    }

    /*Creo canale di comunicazione tra figlio e nipote*/
    if(pipe(pipeFiglioNipote) < 0){
       printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
       exit(4);
    }

    if((outfile=open("/tmp/RISULTATO",O_RDWR|O_APPEND|O_CREAT,PERM)) < 0){
       printf("Errore nella creazione del file\n");
       exit(5);
    }

    write(outfile,"\n",1);
    write(outfile,argv[1], strlen(argv[1]));
    write(outfile,"\n",1);

    for(n=0; n< N; n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(6);
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

            /*Creo canale di comunicazione tra padre e filgio*/
            if(pipe(pipeFiglioNipote) < 0){
               printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
               exit(-1);
            }

            if ((pid = fork()) < 0) { /* creazione con verifica del processo nipote */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
             /* codice eseguito dal processo nipote*/
               
             /* Chiusura pipe che non servono */
                close(pipeFiglioNipote[0]);
                close(pipePadreFiglio[n][1]);

                /* Redirezione del output */
                close(1);
                dup(pipeFiglioNipote[1]);

                close(pipeFiglioNipote[1]);

                /* Eseeguo il comando  */
                execlp("ls","ls", "-li",(char*)0);
                
            /* NON SI DOVREBBE MAI TORNARE QUI !*/
               exit(-1);
            }


            close(pipeFiglioNipote[1]); /* Chiusura delle pipe che non ci servono */

            /* Redirezione del input */

            close(0);
            dup(pipeFiglioNipote[0]);

            /* Redirezione del output */

            close(1);
            dup(pipePadreFiglio[n][1]);

            /* Chiusura pipe inutili */
            close(pipeFiglioNipote[0]);
            close(pipePadreFiglio[n][1]);


            execlp("grep","grep",argv[n+2],(char*)0);

            exit(-1);
        }

    }
    

    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipePadreFiglio[n][1]);
    }


    for(n=0;n<N;n++){
        i=0;
        while (read(pipePadreFiglio[n][0],linea + i,1))
        {
            i++;
        }

        write(outfile,linea,strlen(linea));
        
    }

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
            printf("il figlio %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}