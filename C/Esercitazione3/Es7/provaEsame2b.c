#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/stat.h> //SYStem STATus: stat, fstat, S_IFMT, S_IFDIR, S_IFREG
#include <sys/wait.h> //SYStem WAIT: wait
typedef int pipe_t[2];

int main(int argc, char* argv[]) {
    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    char buffer; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    pipe_t* piped; /* Canale di comunicazione padre-filgio */
    int n; /* scorre i caratteri */
    int N; /*Numero di caratteri */
    long int cont; /* Numero caratteri contati*/
    

    /*Controllo parametri LASCO*/
    if (argc < 4){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
        exit(1);
    }
    
  

    N=argc-2;

   /* allocazione dell'array di N pipe */
  	piped = (pipe_t *)malloc(N * sizeof(pipe_t));
  	if (piped == NULL) {
        printf("Errore nella allocazione array dinamico per le pipe\n");
    	exit(3);
  	}
    /* creazione delle N pipe figli-padre */
  	for (n=0; n < N; n++){
    	if (pipe(piped[n]) < 0)
    	{ 	printf("Errore nella creazione della pipe di indice n = %d\n", n); 
      		exit(4);
    	}
    }


    for(n=0; n < N; n++){
        /*creao tanti figli quanti caratteri */

        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(4);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/  
            
            /* chiusura dei lati delle pipe non usati nella comunicazione con il padre */
      		for (int k = 0; k < N; k++)
      		{	/* ogni figlio NON legge da nessuna pipe e scrive solo sulla sua che e' la n-esima! */
        		close(piped[k][0]);
        		if (k != n){
          			close(piped[k][1]);
                }
      		}


            /* controllo che il parametro sia un file */
            if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(2);
            }          
            
            cont = 0L;

            while (read(fd,&buffer,1) > 0)  {
                if (buffer == argv[n+2][0]) {  
                    cont++;
                }
                
            }

            write(piped[n][1], &cont, sizeof(cont));            
            exit(argv[n+2][0]);
        }


    }


    for (n=0; n < N; n++){
    		close(piped[n][1]);
  	}
    /* legge dalle pipe i messaggi */
  	for (n=0; n < N; n++)
  	{
    		if (read(piped[n][0], &cont, sizeof(cont)) > 0)
      			printf("Trovate %ld occorrenze del carattere %c nel file %s\n", cont, argv[n+2][0], argv[1]);	/* i long int si stampano con %ld */
  	}
    for(n=0;n<N;n++){

        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito\n");
            exit(5);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(6);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("DEBUG-PADRE il figlio %d ha ritornato %c\n",pidFiglio,ritorno);
        }
    
    }


    exit(0);
}