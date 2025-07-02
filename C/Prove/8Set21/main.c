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
    int fd;                    /* file descriptor */
    char Cx;                   /* variabile che conterrà un singolo carattere */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,linee,i,j,k,L1,L2;                     /* Contatore */
    pipe_t* p;                 /* Canale di comunicazione padre-filgio */


    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
        exit(-1);
    }
    N=argc-1;

    /* allocazione dell'array di N pipe */
    p = (pipe_t *)malloc(2*N * sizeof(pipe_t));
    if (p == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(p[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }

    // /*Creazione del file */
    // if ((fw=creat("dicandia.log", PERM)) < 0){
    //    printf("DEBUG-FILGIO Impossibile creare il file %s\n", "dicandia.log" );
    //    exit(-1);
    // }
    // /* chiudiamo il file creato dato che comunque il figlio non lo usa */

    for(n=0;n<N*2;n++){


        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           


            /* Chiusura del canale di lettura di N pipe */
            for(int k=0;k<N;k++){
               close(p[k][0]);
               if(k != n){
                    close(p[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }


            if(n%2 == 0){ /* FIGLI PARI */

                /* controllo che il parametro sia un file */
                if((fd=open(argv[n/2 + 1],O_RDONLY)) < 0 ){ //APRO IL FILE
                   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n/2 + 1]);
                   exit(-1);
                }


                i=0;
                linee=0;
                ritorno = 0;
                while (read(fd,&Cx,1) == 1){

                    if(Cx == '\n'){
                        linee++;
                        if(linee%2 == 0){
                            if((write(p[n][1],&i,sizeof(i))) < 0){ /* Scrivo */
                               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
                               exit(-1);
                            }
                            if(i > ritorno){
                                ritorno = i;
                            }
                        }

                        i=0;
                    }else i++;

                }
                

            }else { /* FIGLI DISPARI */

                /* controllo che il parametro sia un file */
                if((fd=open(argv[(n+1)/2],O_RDONLY)) < 0 ){ //APRO IL FILE
                   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[(n+1)/2]);
                   exit(-1);
                }




                i=0;
                linee=0;
                ritorno = 0;
                while (read(fd,&Cx,1) == 1){

                    if(Cx == '\n'){
                        linee++;
                        if((linee%2) != 0){
                            if((write(p[n][1],&i,sizeof(i))) < 0){ /* Scrivo */
                               printf("ERRORE: qualcosa è andato storto nella scrittura \n");
                               exit(-1);
                            }
                            if(i > ritorno){
                                ritorno = i;
                            }
                        }

                        i=0;
                    }else i++;

                }
               

            }
            exit(ritorno);


        }
        

    }


    /* Chiusura del canale di scrittura di N pipe */
  for (n=0; n < 2*N; n++){
		close(p[n][1]);
   }
   
   for(i=0;i<N;i++){

        //printf("LUNGHEZZE FILE %s\n",argv[i + 1]);
        
        linee=1;
        do{
            /* il padre recupera le lunghezze delle linee da ogni figlio dispari e pari */
	       	k=read(p[i*2+1][0], &L1, sizeof(L1));
			j=read(p[i*2][0], &L2, sizeof(L2));
        
            if(k != 0){
                printf("    Lunghezza della linea %d: %d k:%d\n",linee,L1,k);
                linee++;
            }
        
            if(j != 0){
                printf("    Lunghezza della linea %d: %d j:%d\n",linee,L2,j);
                linee++;
            }

        }while (j||k);
      
        

    }


    for(n=0; n<N*2; n++){
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