#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait

typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
	

	/*------ VARIABILI USATE ------*/
	int fdw,fd;                    /* file descriptor */
	char chs[2];               /* variabile che conterrà un singolo carattere */
	int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
	int status;                /* Variabile che conterrà lo stato di un figlio */
	int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
	int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
	int N;                     /* Numero di figli */
	int n,finito;                     /* Contatore */
	int nread;
	int nwrite;
	pipe_t* pipeFiglioPadre;   /* Canale di comunicazione padre-filgio */



	/*Controllo parametri LASCO*/
	if (argc < 3){
		printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
		exit(-1);
	}


	N=argc-2;

	/* allocazione dell'array di N pipe */
	pipeFiglioPadre = (pipe_t *)malloc(N * sizeof(pipe_t));
	if (pipeFiglioPadre == NULL) {
	   printf("Errore nella allocazione array dinamico per le pipe\n");
	   exit(3);
	}
	/* Creazione delle N pipe */
	for(n=0;n<N;n++){
	   if(pipe(pipeFiglioPadre[n]) < 0 ){
		  printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
		  exit(3);
	   }
	}

	/* controllo che il parametro sia un file */
	if((fdw=open(argv[N+1],O_WRONLY)) < 0 ){ //APRO IL FILE
	   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[N+1]);
	   exit(-1);
	}


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
			close(pipeFiglioPadre[k][0]);
			if(k != n){
				 close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
			}
		 }

		 /* controllo che il parametro sia un file */
		 if((fd=open(argv[n+1],O_RDONLY)) < 0 ){ //APRO IL FILE
			printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
			exit(-1);
		 }

		 ritorno = 0;
		 while((nread = read(fd,chs,2*sizeof(char))) != 0){ /* Leggo il contenuto del file */
			
			nwrite= write(pipeFiglioPadre[n][1],chs,nread);
			if(nwrite != nread){ /* Leggo il contenuto del file */
			   printf("Errore nella scrittura 86\n");
			   exit(-1);
			}
			ritorno+=nwrite;

		 }
	   	 
		 close(fd); 


		 exit(ritorno);
		}


	}


	/* Chiusura del canale di scrittura di N pipe */
	for(n=0;n<N;n++){
	   close(pipeFiglioPadre[n][1]);
	}

	finito=0;/* Supponiamo che nessun figlio ha finito */
	while(!finito){ /* Finché tutti i figli non finiscono */

		finito=1; /* Ipotizziamo che abbaiano finito tutti */
		for(n=0;n<N;n++){

			if((nread = read(pipeFiglioPadre[n][0],chs,2*sizeof(char))) > 0){ /* Leggo il contenuto del file */
				/* Se la letture è andata bene */

				write(fdw,chs,nread*sizeof(char)); /* Scrivo in append sul file */
				write(fdw,"\n",sizeof(char)); /* Scrivo in append sul file */
				finito=0; /* Almeno uno filgio non ha finito di leggere */

			}

		}


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
			printf("Il figlio %d con PID %d ha ritornato %d\n",n,pidFiglio,ritorno);
		}

	}

	close(fdw);
	exit(0);
}