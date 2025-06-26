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
    int fd,fcreato;                    /* file descriptor */
    char Cx,linea[250];                   /* variabile che conterrà un singolo carattere */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,riga,i,n1,n2;                     /* Contatore */
    pipe_t* dispari;   /* Canale di comunicazione padre-filgio */
    pipe_t* pari;   /* Canale di comunicazione padre-filgio */


    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }


    N=argc-1;


    /*Creazione del file */
    if ((fcreato=creat("dicandia.log", PERM)) < 0){
       printf("DEBUG-FILGIO Impossibile creare il file %s\n", "DICANDIA");
       exit(-1);
    }


    /* allocazione dell'array di N pipe */
    pari = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pari == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pari[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }
    
    
    /* allocazione dell'array di N pipe */
    dispari = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (dispari == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(dispari[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }




    for(n=0;n<N*2;n++){


        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/


            if(n%2){ /* Se pari*/

                /* Chiusura del canale di lettura di N pipe */
                for(int k=0;k<N;k++){
                   close(pari[k][0]);
                   if(k != n){
                        close(pari[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                   }
                }

                close(dispari[n][0]);
                close(dispari[n][1]);

            }else { /* Se dispari*/


                /* Chiusura del canale di lettura di N pipe */
                for(int k=0;k<N;k++){
                   close(dispari[k][0]);
                   if(k != n){
                        close(dispari[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                   }
                }

                close(pari[n][1]);
                close(pari[n][0]);


            }



            /* controllo che il parametro sia un file */
            if((fd=open(argv[n/2+1],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n/2+1]);
               exit(-1);
            }

            riga=0;
            i=0;
            ritorno=-1;
            while((read(fd,&Cx,1)) > 0){ /* Leggo il contenuto del file */
               
                if(Cx=='\n'){
                    i++;
                    printf("n:%d riga:%d\n",n,riga);
                    if(((n%2) == 0) && ((riga%2) == 0)){ /* Pari */
                        printf("DEBUG-FIGLIO %d scrivo sulla pipe la riga %d\n",n,riga);
                        if(write(pari[n/2][1],&i,sizeof(i)) < 0){ /* Leggo il contenuto del file */
                           printf("Errore nella scrittura 139 %d\n",n);
                           exit(-1);
                        }

                        if(ritorno < i){
                            ritorno = i;
                        }

                    }else if(((n%2) != 0) && ((riga%2) != 0)){ /* Dispari */
                        printf("DEBUG-FIGLIO %d scrivo sulla pipe la riga %d\n",n,riga);

                        if((write(dispari[n/2][1],&i,sizeof(i))) < 0){ /* Leggo il contenuto del file */
                           printf("Errore nella scrittura 146\n");
                           exit(-1);
                        }
                    

                        if(ritorno < i){
                            ritorno = i;
                        }

                    }
                    riga++;
                    i=0;
                }else i++;
                


            }           
           exit(ritorno);
        }


    }


    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pari[n][1]);
       close(dispari[n][1]);
    }


    for(n=0; n < N; n++){
      
        printf("Lunghezza righe del file %d\n",1);
        i=0;
        while (read(dispari[n][1],&n1,sizeof(int)) || read(pari[n][1],&n2,sizeof(int)))
        {
            i++;
            
            if(n1){
                printf("la lunghezza della riga %d è %d\n",i,n1);
            }
            if(n2){
                printf("la lunghezza della riga %d è %d\n",i,n2);
            }

        }

    }



    for(n=0;n<N*2;n++){

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
            sprintf(linea,"Il filgio %d con pid %d ha ritornato %d\n",n,pidFiglio,ritorno);
            write(fcreato,linea,sizeof(linea));
        }

    }

    exit(0);
}