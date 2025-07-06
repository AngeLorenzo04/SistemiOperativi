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
typedef int pipe_t[2]; /* Tipo per creare più pipe */
#define MSGSIZ 250


int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio,pidNipote;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,i;                     /* Contatore */
    char line[MSGSIZ];
    char token;
    pipe_t* pipeline;   /* Canale di comunicazione padre-filgio */

    /*Controllo parametri STRETTO*/
    if (argc != 4){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 3 parametri\n");
        exit(-1);
    }   



    /* controllo che il parametro sia un valore numerico */
    N = atoi(argv[1]);
    if (N <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[1]);
       exit(-1);
    }

    /* controllo che il parametro sia un file */
    if((fd=open(argv[2],O_RDWR)) < 0 ){ //APRO IL FILE
       printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[2]);
       exit(-1);
    }

    /* allocazione dell'array di N pipe */
    pipeline = (pipe_t *)malloc((N-1)* sizeof(pipe_t));
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


    printf("Sono il padre e creero' %d figli\n",N);

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
            if(n == N-1 || k != n) close(pipeline[k][1]); 
            if(n == 0 || k != n-1) close(pipeline[k][0]);
         }


         /* Salvo la lineaa a cui il processo è assenato*/
         i=0;
         while (read(fd,&line[i],1) == 1){
            
            if(line[i] == '\n'){
                line[i]='\0';
                break;
            }else i++;

         }


         if(n != 0){
            read(pipeline[n-1][0],&token,1); /*Aspetta che il figlio precedente abbia finito*/
         }

         if ((pid = fork()) < 0) { /* creazione con verifica del processo nipote */
             /* fork fallita */
             printf("Errore in fork\n");
             exit(-1);
         }
         if (pid == 0) {
          /* codice eseguito dal processo nipote*/
          /* Chiudo le pipe rimaste aperte */
          if(n != N-1)  close(pipeline[n][1]);
          if(n != 0)    close(pipeline[n-1][0]);

            /*Ridirezione dello standard err*/
            close(2);
            open("/tmp/null",O_WRONLY);

            execlp("chmod","chmod", argv[3], line, (char*)0);


            /*QUI NON SI DOVREBBE MAI ARRIVARE*/
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
             if(ritorno == 0) printf("Il nipote %d con pid %d ha ritornato %d\n",n,pidNipote,ritorno);
             if(ritorno != 0) printf("Il nipote %d con pid %d ha ritornato %d qualcosa NON è OK\n",n,pidNipote,ritorno);
         }


         if(n != N-1){

            if((write(pipeline[n][1],&token,1)) < 0){ /* Mando al figlio successivo il via libera */
               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
               exit(-1);
            }

         }
           exit(ritorno);
        }
        


    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipeline[n][1]);
       close(pipeline[n][0]);
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