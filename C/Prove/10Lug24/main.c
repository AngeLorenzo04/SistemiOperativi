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
#include <time.h>
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */
typedef char line_t[250]; /* Tipo per contenere un riga di testo*/

int mia_random(int n)
{
    int casuale;
    casuale = rand() % n;
    casuale++;
    return casuale;
}


int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                    /* file descriptor */
    char Cx;                  /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                      /* Variabile che conterrà il valore di ritorno di fork */
    int status;                   /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;                /* Variabile che conterrà il pid del figlio*/
    int ritorno;                  /* Variabile che conterrà il ritrono del filgio*/
    int n;                        /* Variabile contatore */
    int N;                        /* Numero di file */
    int num;                      /* Serve per l'atoi */
    int r;                        /* Numero random */
    int j;
    int L;
    line_t *random;               /* Array che conterrà le righe random */
    pipe_t* pipeLine;             /* Pipe line */


    /* Controllo dei parametri */


    /*Controllo parametri LASCO*/
    if (argc < 5){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 4 argomenti\n");
        exit(1);
    }

    N=argc - 1;
    for(n=0;n<N;n++){

        if(n%2){
            if((num=atoi(argv[n+1])) <= 0){
                printf("ERRORE: il parametro %s non e' valido\nSUGGERIMENTO: inserire un valore numerico positivo\n",argv[n+1]);
                exit(2);
            }
        }

    }

    N=(argc - 1)/2;
    printf("DEBUG-N %d\n",N);
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


    /* Allocazione dell' array per le righe random */
    random=(line_t*)malloc(N*sizeof(line_t));
    if(random == NULL){
        printf("Errore nell allocazione array dineamico\n");
        exit(4);
    }


    for(n=0;n<N;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(5);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
         printf("DEBUG-FIGLIO creato figlio %d con pid %d\n",n+1,getpid());
           if((L=atoi(argv[n *2 + 2])) == 0){
            printf("ERRORE: atoi andata male\n");
            exit(-1);
           }
         /* Chiusura del canale di lettura di N pipe */
           for(int k=0;k<N;k++){
              if(k != n - 1 || n == 0){
                close(pipeLine[k][0]); /* Chiusura dei canali di Lettura che non ci interessano */
              }
              if(k != n){
                close(pipeLine[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
              }
           }



           srand(time(NULL)); /* Inizializzo il seme */
        

           r=mia_random(L); /* Genero il numero random */


           if(n != 0){
                num = read(pipeLine[n-1][0],random,N*sizeof(line_t));
                if(num < 1){ /* Leggo la pipeLine */
                    printf("ERRORE NELLA LETTURA DELLA PIPE figilo %d\n",n+1);
                    exit(-1);
                } 
           }

           

           /* controllo che il parametro sia un file */
           if((fd=open(argv[n*2+1],O_RDONLY)) < 0 ){ //APRO IL FILE
              printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n*2+1]);
              exit(-1);
           }

           num=0; /* uso la variabile num per contare il numre di righe */
           j=0;
           while (read(fd,&Cx,1)) {

                if(Cx == '\n'){
                    num++;
                }

                if(num == r - 1){
                    while (read(fd,random[n] + j,1)) {
                        if(random[n][j] == '\n'){
                            random[n][j] = '\0';
                            printf("\nDEBUG-FIGLIO %d\nriga: %d\ncontenuto: %s\n\n",n+1,r,random[n]);
                            write(pipeLine[n][1],random,N*sizeof(line_t));
                            break;
                        }else j++;
                    }
                    break;
                }
            
           }
           
           close(fd);

           exit(r);
        }
    }



    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipeLine[n][1]);
       if(n != N - 1){
            close(pipeLine[n][0]);
       }
    }

    /* Stampo le righe */
    read(pipeLine[N-1][0],random,N*sizeof(line_t));
    for(n=0; n<N; n++){
        
        printf("\nLinea ricevuta dal figlio %d\n",n);
        printf("\n%s\n",random[n]);
        
    }

    printf("\n");

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
            printf("DEBUG-PADRE il figlio %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }


    exit(0);
}