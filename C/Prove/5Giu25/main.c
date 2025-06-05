#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
#define LSIZE 250 /* Lunghezza massima di una riga */
typedef int pipe_t[2]; /* Tipo per creare più pipe */


int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int openfile;               /* file descriptor */
    char LINE[LSIZE];           /* Buffer dove verra memorizzato il contenuto di una generica riga */
    char token='v';
    int pid;                    /* Variabile che conterrà il valore di ritorno di fork */
    int status;                 /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio,pidNipote;              /* Variabile che conterrà il pid del figlio*/
    int ritorno;                /* Variabile che conterrà il ritrono del filgio*/
    int M;                      /* Numero di figli */
    int m,cntn,i;               /* Contatore */
    pipe_t* pipeLine;           /* Canale di comunicazione pipeLine */


    /*Controllo parametri STRETTO*/
    if (argc != 4){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 3 parametri\n");
        exit(1);
    }


    /* controllo che il parametro sia un valore numerico */
    M = atoi(argv[1]);
    if (M <= 0){
       printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[1]);
       exit(2);
    }

    /* controllo che il parametro sia un file */
    if((openfile=open(argv[2],O_RDONLY)) < 0 ){ //APRO IL FILE
       printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[2]);
       exit(3);
    }
    close(openfile); /* lo apro successivamente nel processo figlio per essere sicuro di non avere problemi con il cursore*/

    /* Sul terzo parametro non c'e bisogno di controlli */


    /* allocazione dell'array di N pipe */
    pipeLine = (pipe_t *)malloc(M * sizeof(pipe_t));
    if (pipeLine == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(4);
    }
    /* Creazione delle N pipe */
    for(m=0;m<M;m++){
       if(pipe(pipeLine[m]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(5);
       }
    }

    printf("DEBUG-sono il padre con pid %d e creero' %d processi\n",getpid(),M);

    for(m=0;m<M;m++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(6);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
           
            printf("\n FIGLIO %d pid %d\n\n",m,getpid());


            for(int k=0;k<M;k++){
                if( k!= m-1 || m == 0){
                    close(pipeLine[k][0]); /* Chiusura dei canali di lettura che non ci interessano */

                }
               if(k != m){
                    close(pipeLine[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
               }
            }

            openfile=open(argv[2],O_RDONLY);


            while (read(pipeLine[m - 1][0],&token,1)|| m==0) /* Finchè il filgio precedente non da il via la read blocca l'esecuzione */
            {
                cntn=0; /* conto quanti terminatori di riga leggo in modo da individuare e salvare la riga scelta */
                i=0;
                while (read(openfile,LINE + i,1)){ /* Leggo carattere per carattere */
                    if(LINE[i]== '\n'){

                        cntn++; /* trovato il terminatore aggiorno il contatore */
                    }
                    if(cntn == m){ /* se il contatore ha individuato la riga designata */

                        while (read(openfile,LINE + i,1)) /* la salvo nel buffer LINE*/
                        {
                            if(LINE[i] == '\n'){
                                break;
                            }else i++;
                        }
                        LINE[i] = '\0';
                        break;


                    }else i = 0; /* questo mi permette di occupare solo il primio carattere del buffer */

                }
                break; /* indispensabile nel caso in cui m==0 si rischia di entrare in un loop infinito*/
            }

            write(pipeLine[m][1],&token,1); /* Do il comando al figlio successivo di proseguire */

            if ((pid = fork()) < 0) { /* creazione con verifica del processo Nipote */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
             /* codice eseguito dal processo Nipote*/
               
             /* File memorizzato in LINE quindi non serve redirigere l'input */
             /* Comando chmod non da messaggi in outpu in caso di successo*/
             /* Comando chmod da un messaggio su standard err quando il comando non e' valido per cui redirigo err */
             close(2);
             open("/dev/null",O_WRONLY);
             /* Mi interessa sapere solo se il comando e' andato a buon fine */

             /* uso execlp perchè il comando chmod si trova nella variabile ambiente PATH*/
             execlp("chmod","chmod",argv[3],LINE,(char*)0);


             /* IL NIPOTE E' DIVENTATO IL COMANDO CHMOD NON DEVE RITORNARE !!*/
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
            }

           exit(ritorno); /* Ritorno il risultato ricevuto dal nipote */
        }

    }

    /* Chiusura del canale inutilizzati */
    for(m=0;m<M;m++){
       close(pipeLine[m][1]);
       close(pipeLine[m][0]);
    }


    for(m=0;m<M;m++){
        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(6);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido
           printf("Figlio terminato %d in modo anomalo\n",m);
           exit(7);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            if(ritorno != 0){ /*Separiamo i casi in cui sia andato tutto bene o no*/
                printf("il nipote del filglio con pid %d ha ritornato %d e quindi ha fallito l'esecuzione del comando chmod oppre se 255 il nipote o il figlio ha avuto dei problemi\n",pidFiglio,ritorno);
            }else{
                printf("Il nipote del figlio con pid %d ha ritornato %d, quindi tutto OK!\n",pidFiglio,ritorno);
            }
        }


    }



    exit(0);
}