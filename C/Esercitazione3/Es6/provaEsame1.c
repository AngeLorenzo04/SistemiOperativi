#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait

int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    char buffer; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int piped[2]; /* Canale di comunicazione padre-filgio */
    int N; /* Numero di figli */
    int n; /* indice dei figli */
    int tot; /*Numero caratteri passati*/

    /*Controllo parametri LASCO*/
    if (argc < 3){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2");
        exit(1);
    }

    /*Creo canale di comunicazione tra padre e filgio*/
    if(pipe(piped) < 0){
        printf("ERRORE: qualcosa e' andato storto nella creazione della pipe\n");
        exit(2);
    }


    N=argc - 1;
    for(n=0; n<N; n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(3);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
            close(piped[0]);
            if((fd=open(argv[n + 1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n + 1]);
               exit(-1);
            }


            while(read(fd,&buffer,1) == 1){ /* Leggo il contenuto del file */
               if((n+1)%2 == 0){ /*se il file ha indice pari*/
                    if(atoi(&buffer) != 0){ /*se e' un carattere numerico*/
                        write(piped[1],&buffer,1); /* lo SCRIVO sulla pipe*/
                    }
               }else{
                    if(atoi(&buffer) == 0){ /*se e' un carattere alfabetico*/
                        write(piped[1],&buffer,1); /* lo SCRIVO sulla pipe*/
                    }
               }
            }



            exit(buffer);
        }
    }
        close(piped[1]);
        tot=1;
        while(read(piped[0],&buffer,1) == 1){
            
            printf("DEBUG-PADRE %d:%c\n",tot,buffer);
            tot++;

        }

        for(n=0;n<N;n++){
            if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
                printf("ERRORE: wait ha fallito\n");
                exit(4);
            }
            if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
               printf("Figlio terminato in modo anomalo\n");
                exit(5);
            }else { //estraggo il valore passato dal figlio
                ritorno = status >> 8;
                /* selezione degli 8 bit piu’ significativi */
                ritorno &= 0xFF;
                printf("DEBUG-PADRE il figlio %d ha ritornato %c\n",pidFiglio,ritorno);
            }
        }
        printf("DEBUG-PADRE sono stati mandati %d parametri\n",tot - 1);
    

    exit(0);
}