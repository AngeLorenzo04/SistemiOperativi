#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait
#define PERM 777 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char const *argv[])
{
    
    /*------ VARIABILI USATE ------*/
    int fd,fdw;                    /* file descriptor */
    char Cx,name[250];                   /* variabile che conterrà un singolo carattere */
    char* buff;               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int L,  B;                     /* Numero di figli */
    int q,start;                     /* Contatore */
    pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */


    /* F L B */

    /*Controllo parametri STRETTO*/
    if (argc != 4){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 3 parametri\n");
        exit(-1);
    }

    /*Creazione del file */
    sprintf(name,"%s.chiara",argv[1]);
    if ((fdw=creat(name, PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n",name );
       exit(-1);
    }

    /* controllo che il parametro sia un valore numerico */
    L = atoi(argv[2]);
    if (L <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
       exit(-1);
    }

    /* controllo che il parametro sia un valore numerico */
    B = atoi(argv[3]);
    if (B <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[3]);
       exit(-1);
    }

    /* allocazione dell'array di N pipe */
    pipePadreFiglio = (pipe_t *)malloc(B * sizeof(pipe_t));
    if (pipePadreFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(q=0;q<B;q++){
       if(pipe(pipePadreFiglio[q]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    
    
    buff = (char*)malloc(B/L * sizeof(char));
    if(buff == NULL){
    
        printf("Errore nell allcoazione della memoria\n");
        exit(-1);  
    
    }

    for(q=0;q<B;q++){


        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
            
            printf("DEBUG-FIGLIO creazione del figlio %d\n",q);
            
           /* Chiusura del canale di lettura di N pipe */
           for(int k=0;k<B;k++){
              close(pipePadreFiglio[k][0]);
              if(k != q){
                   close(pipePadreFiglio[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
              }
           } 

           
           if((fd=open(argv[1],O_RDONLY)) < 0 ){ //APRO IL FILE
              printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
              exit(-1);
           }

           start=q*L/B;
           lseek(fd,start,SEEK_SET);
           read(fd,buff,L/B);
           write(pipePadreFiglio[q][1],&buff[L/B -1],1);
         

           exit(L/B);
        }

    }

    /* Chiusura del canale di scrittura di N pipe */
    for(q=0;q<B;q++){
       close(pipePadreFiglio[q][1]);
    }
    

    for(q = 0; q < B; q++){

        if((read(pipePadreFiglio[q][0],&Cx,1)) < 0){ /* Leggo il contenuto del file */
           printf("ERRORE: qualcosa è andato storto nella letttura\n");
           exit(-1);
        }

        printf("DEBUG-PADRE scrivo carattere %c\n",Cx);
        if((write(fdw,&Cx,1)) < 0){ /* Leggo il contenuto del file */
           printf("ERRORE: qualcosa è andato storto nella scrittura \n");
           exit(-1);
        }

    }


    for(q=0;q<B;q++){

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
            printf("Il filgio %d ha ritornato %d\n",q,ritorno);
        }


    }

    return 0;
}
