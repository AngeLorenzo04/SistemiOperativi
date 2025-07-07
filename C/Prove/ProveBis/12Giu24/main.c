#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait
#define PERM 777 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd,wfd;                    /* file descriptor */
    char buffer[250];               /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int N;                     /* Numero di figli */
    int n,line,i,size,max;                     /* Contatore */
    pipe_t* p;                 /* Canale di comunicazione figlio-filgio */


    /*Controllo parametri LASCO*/
    if (argc < 2){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 1 argomenti\n");
        exit(-1);
    }

    N=argc-1;


    /* allocazione dell'array di N pipe */
    p = (pipe_t *)malloc(N * sizeof(pipe_t));
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


    for(n=0;n<N*2;n++){

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(-1);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/

        /* Chiusura del canale di lettura di N pipe */  
        for(int k=0;k<N*2;k++){
          
            if(n < N ){

                close(p[k][0]);
                if(k != n) close(p[k][1]);

            }else{
                close(p[k][1]);
                if(k != (2*N-1)-n) close(p[k][0]);

            }
        }

        max=0;
        if(n<N){

            /* controllo che il parametro sia un file */
            if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
               exit(-1);
            }

 
            

            line = 0;
            i=0;
            while (read(fd,&buffer[i],1) == 1){
                if(buffer[i] == '\n'){
                    line++;
                    if(line % 2 != 0){
                        if((write(p[n][1],&i,sizeof(int))) < 0){ /* Scrivo */
                           printf("ERRORE: qualcosa è andato storto nella scrittura 97\n");
                           exit(-1);
                        }
                        buffer[i+1]='\0';
                        if((write(p[n][1],buffer,i)) < 0){ /* Scrivo */
                           printf("ERRORE: qualcosa è andato storto nella scrittura 100\n");
                           exit(-1);
                        }

                        if(i > max){
                            max = i;
                        }
                        
                    }
                    i=0;

                }else i++;


            }

            

        }else{

            strcpy(buffer,argv[2*N-n]);
            strcat(buffer,".max");
            /*Creazione del file */
            if ((wfd=creat(buffer, PERM)) < 0){
               printf("DEBUG-FILGIO Impossibile creare il file %s\n", buffer);
               exit(-1);
            }
         
            /* controllo che il parametro sia un file */
            if((fd=open(argv[(2*N)-n],O_RDONLY)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[(2*N -1)]);
               exit(-1);
            }
            
            i=0;
            line=0;
            while (read(fd,&buffer[i],1) == 1){
                
                if(buffer[i] == '\n'){

                    line++;
                    if(line % 2 == 0){
                        if((read(p[(2*N-1)-n][0],&size,sizeof(size))) < 0){ /* Leggo */
                           printf("ERRORE: qualcosa è andato storto nella letttura  149\n");
                           exit(-1);
                        }
                        if( i > size){ /* Se la riga pari è più lunga la scrivo */
                            write(wfd,buffer,i); 
                        }

                        if((read(p[(2*N-1)-n][0],buffer,size)) < 0){ /* Leggo */
                           printf("ERRORE: qualcosa è andato storto nella letttura 158\n");
                           exit(-1);
                        }

                        if(size > i){ /* se la riga dispari è più lunga la scrivo*/
                            write(wfd,buffer,size);
                        }

                        write(wfd,"\n",1);

                        if(max < i){
                            max = i;
                        }

                    }
                    i=0;

                }else i++;

            }
            


        }
           
           exit(max);
        }

    
    }

    for(n=0;n<N;n++){
        close(p[n][0]);
        close(p[n][1]);
    }
    for(n=0; n<2*N ;n++){

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