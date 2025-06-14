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

typedef struct 
{

    char c1[12]; /* TROVATO o NON TROVATO */
    int c2; /* PID NIPOTE */

} Strut;



int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio,pidNipote;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n;                     /* Contatore */
    pipe_t* pipeLine;   /* Canale di comunicazione padre-filgio */
    Strut *S;

    /*Controllo parametri LASCO*/
    if (argc < 4){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(-1);
    }


    N = argc - 2;


    

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
            if(n == 0 || k != n - 1){
                close(pipeLine[k][0]); /* Chiusura dei canali di scrittura che non ci interessano */
            }
            if(k != n){
                close(pipeLine[k][1]);
            }
         }


         if ((pid = fork()) < 0) { /* creazione con verifica del processo Nipote */
             /* fork fallita */
             printf("Errore in fork\n");
             exit(-1);
         }
         if (pid == 0) {
          /* codice eseguito dal processo Nipote*/
            
            if(n != 0) close(pipeLine[n-1][0]);
            close(pipeLine[n][1]);

            close(1);
            open("/dev/null",O_WRONLY);

            close(2);
            open("/dev/null",O_WRONLY);

            execlp("grep","grep",argv[n+2],argv[1],(char *)0);

            
            exit(-1);
         }
         
         if((pidNipote = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
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
             printf("DEBUG-FILGIO il nipote %d pid %d ha ritornato %d\n",n,pidNipote,ritorno);
         }


         S = (Strut*)malloc(N*sizeof(Strut));
         if(S == NULL){
             printf("Errore nel allocazione di memoria\n");
         }

          if (n != 0) {
            
            if((read(pipeLine[n-1][0],S,N * sizeof(Strut))) < 0){ /* Leggo il contenuto del file */
               printf("Errore nella lettura ");
               exit(-1);
            }

         }

         S[n].c2 = pidNipote;
         if(ritorno == 0){
            strcpy(S[n].c1,"TROVATO");
         }else strcpy(S[n].c1,"NON TROVATO");


         if((write(pipeLine[n][1],S,N*sizeof(Strut))) < 0){ /* Leggo il contenuto del file */
            printf("Errore nella lettura ");
            exit(-1);
         }
           
           exit(ritorno);
        }

    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipeLine[n][1]);
       if(n != N - 1) close(pipeLine[n][0]);
    }


    S=(Strut*)malloc(N*sizeof(Strut));
    if( S == NULL ){

        printf("Errore nell allocazione della array\n");
        exit(-1);

    }


    if((read(pipeLine[N-1][0],S,N * sizeof(Strut))) < 0 ){ /* Leggo il contenuto del file */
       printf("Errore nella lettura\n");
       exit(-1);
    }


    for(n=0;n<N;n++){
        if(strcmp(S[n].c1,"TROVATO") == 0){
            printf("il nipote %d del filgio %d ha trovato la stringa cercata \n",S[n].c2,n);
        }else printf("il nipote %d del filgio %d non ha trovato la stringa cercata \n",S[n].c2,n);
    }


    for (n=0;n<N;n++)
    {
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
            printf("Il figlio %d ha ritornato %d\n",n,ritorno);
        }
    }
    


    exit(0);
}