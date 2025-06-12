#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait

typedef int pipe_t[2]; /* Tipo per creare più pipe */

typedef struct 
{

    long int c1; /* Deve contenere il valore minimo di linee */
    int c2; /* Deve contenere l' indice d'ordine del processo */

} struct_t;


int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd;                     /* file descriptor */
    char linea[250];            /* Buffer dove verra memorizzata ogni linea */
    int pid;                    /* Variabile che conterrà il valore di ritorno di fork */
    int status;                 /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;              /* Variabile che conterrà il pid del figlio*/
    int ritorno;                /* Variabile che conterrà il ritrono del filgio*/
    int N;                      /* Numero di figli */
    int n,i;                      /* Contatore */
    long int cur_tot, min;           /* Variabili per il conteggio del minimo */
    pipe_t* pipeLine;           /* Canale di comunicazione padre-filgio */
    pipe_t* pipePadreFiglio;    /* Canale di comunicazione padre-filgio */
    struct_t S;                 /* Struct */


    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(1);
    }

    N=argc-1; /*Aggiorno al variabile N con il numero di parametri passati*/

    /* controllo che il parametro sia un file */
    for(n=0;n<N;n++){
        if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
           printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
           exit(2);
        }
        close(fd);
    }


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


    for(n=0;n<N;n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(3);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/

         for(int k=0;k<N;k++){
            close(pipePadreFiglio[k][1]); /* Chiudo tutti i canali di scrittura del padre */
            if(n == 0 || k != n-1){
                close(pipeLine[k][0]); /* Chiudo le letture della pipeline che non servono */
            }
            if(k != n){
                close(pipeLine[k][1]); /* Chiudo le scritture della pipeline che non servono */
            }
         }



         /* controllo che il parametro sia un file */
         if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
            printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
            exit(-1);
         }
         
         /* Conto il numero di righe */
         i=0;
         cur_tot = 0;
         while (read(fd,&linea[i],1)){
            if(linea[i] == '\n'){
                cur_tot++;
                i=0;
            }else i++;
         }
         
         /* Salvo nella struct se sono al primo giro altrimenti faccio un confronto */
         if(n == 0){
            S.c1 = cur_tot;
            S.c2 = n;
         }else{
            if((read(pipeLine[n-1][0],&S,sizeof(S))) < 0){ /* Leggo dalla pipe del filgio precedente */
                printf("Errore nella lettura 134\n");
                exit(-1);
            }

            if(S.c1 > cur_tot){ /* Se ho meno rige aggiorno il valore nella struct */
                S.c1 = cur_tot;
            }

         }        
        
        if((write(pipeLine[n][1],&S,sizeof(S)))<0){ /* Mando la struct al figlio successivo */
        
            printf("Erroren nella scrittura 143\n");
        
            exit(-1);
        
        }

         /* Aspetto che il padre mi mandi il segnale di via libera */
         if((read(pipePadreFiglio[n][0],&min,sizeof(min))) < 0){

            printf("Errore nella lettura 152\n");
            exit(-1);

         }


         printf("\nSono il filigo %d il padre ha mandato: %ld ho pid: %d\n",n,min,getpid());

           exit(cur_tot);
        }
    }


    /* Chiusura del canale di lettura di N pipe */
    for(int k=0;k<N;k++){
       close(pipePadreFiglio[k][0]); /* Chiusura dei canali di lettura che non ci interessano*/
       close(pipeLine[k][1]);
       if(k != N-1){
            close(pipeLine[k][0]);
       }
    }

    if((read(pipeLine[N-1][0],&S,sizeof(S))) < 0){

        printf("Lettura andata male 179\n");
        exit(4);

    }

    min = S.c1;
    for(n=0;n<N;n++){
        if(write(pipePadreFiglio[n][1],&min,sizeof(min)) < 0){
            printf("Scritura andata male 187\n");
            exit(5);
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
            printf("Il filgio %d ha ritornato %d\n",pidFiglio,ritorno);
        }


    }



    exit(0);
}