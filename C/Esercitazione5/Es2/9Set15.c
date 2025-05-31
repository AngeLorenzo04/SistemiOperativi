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
#include <signal.h> //ISoType: isdigit, isalpha, isalnum
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */


void handler(int signo)
{
    printf("DEBUG-Sono il padre %d e ho ricevuto il segnale %d\n", getpid(), signo);
}

int main(int argc, char* argv[]) {
	/*------ VARIABILI USATE ------*/
	int AF;                    /* file descriptor */
	int Fn;					   /* File descriptor */
	char Cx;                   /* variabile che conterrà un singolo carattere del filgio */
	char c;					   /* Variabile che conterrà un singolo carattere del padre */
	int* pid;                  /* Variabile che conterrà il valore di ritorno di fork */
	int status;                /* Variabile che conterrà lo stato di un figlio */
	int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
	int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
	int n;					   /* Variabile per scorrere */
	int N;                	   /* Numero di figli */
	int* confronto;
	char token='v';
	pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
	pipe_t* pipeFiglioPadre;


	/*Controllo parametri LASCO*/
	if (argc < 3){
		printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
		exit(1);
	}


	N=argc-2;

	/* il padre installa il gestore handler per il segnale SIGPIPE */
    signal(SIGPIPE, handler);

	/*allocazione del array confronto */
	confronto=(int*)malloc(N*sizeof(int));
	for(n=0;n<N;n++){
		confronto[n]=1;
	}
	/* allocazione dell array di pid*/
	if((pid= (int*)malloc(N * sizeof(int))) == NULL){
		printf("Errore nell allocazione array dinamico per pid\n");
		exit(2);
	}

	/* allocazione dell'array di N pipe */
	pipePadreFiglio = (pipe_t *)malloc((N+1) * sizeof(pipe_t));
	if (pipePadreFiglio == NULL) {
	   printf("Errore nella allocazione array dinamico per le pipe\n");
	   exit(3);
	}
	/* Creazione delle N pipe */
	for(n=0;n<N+1;n++){
	   if(pipe(pipePadreFiglio[n]) < 0 ){
		  printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
		  exit(4);
	   }
	}



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

	for ( n = 0; n < N; n++)
	{
		
		if ((pid[n] = fork()) < 0) { /* creazione con verifica del processo figlio */
			/* fork fallita */
			printf("Errore in fork\n");
			exit(2);
		}
		if (pid[n] == 0) {

			/* Chiusura del canale di lettura di N pipe */
			for(int k=0;k<N;k++){
			   close(pipePadreFiglio[k][1]); /* blocco tutte le scritture del padre-figlio*/
			   close(pipeFiglioPadre[k][0]); /* blocco tutte le letture del figlio-padre */
			   if(k != n){
					close(pipePadreFiglio[k][0]); /* Chiusura dei canali di lettura che non ci interessano */
					close(pipeFiglioPadre[k][1]); /* Chiusura dei canali di scrittura che non ci interessano*/
			   }
			}
		 
			/* controllo che il parametro sia un file */
			if((Fn=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
			   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
			   exit(-1);
			}

			while (read(pipePadreFiglio[n][0],&token,1)){

				if(token == 't'){ /* Leggendo 't' il padre ha detto di terminare */
					break;
				}

				read(Fn,&Cx,1); /* Leggo il carattere dal file e lo salvo in Cx*/
				write(pipePadreFiglio[n][1],&Cx,1); /* Scrivo al padre il carattere che ho trovato */
				printf("DEBUG: Sono il figlio %d di indice %d e ho finito di leggere il file %s\n", getpid(), n, argv[n+1]); 
			}


		 exit(0);
		
		}

	}

	/* Chiusura del canale di scrittura di N pipe */
	for(n=0;n<N;n++){
	   close(pipeFiglioPadre[n][1]); /* chiudo la scrittura figlio padre*/
	   close(pipePadreFiglio[n][0]); /* chiudo la lettura padre figlio */
	}

	/* controllo che il parametro sia un file */
	if((AF=open(argv[argc-1],O_RDWR)) < 0 ){ //APRO IL FILE
	   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[argc-1]);
	   exit(-1);
	}


	while (read(AF, &c, 1)){
		for(n=0;n<N;n++)
		{
			/* il padre manda l'indicazione di leggere ad ogni figlio per ogni carattere solo se confronto e' ancora 1: ricordiamo che token e' inizializzato a 'v'! */
			if (confronto[n])
			{
				write(pipePadreFiglio[n][1], &token, 1);
				/* il padre riceve il carattere letto dal figlio e lo confronta con il suo */
				read(pipeFiglioPadre[n][0],&Cx,1);
               			/* printf("SONO IL PADRE: HO LETTO il carattere %c e il figlio %c\n", ch, c);  */
				if (Cx != c)
              			{
					/* i caratteri sono diversi e quindi bisogna resettare il valore corrispondente di confronto */
					/* printf("DEBUG-SONO IL PADRE: HO LETTO il carattere %c e il figlio %c e quindi mi segno che non devo piu'ù mandare indicazione a questo figlio %d\n", ch, c, pid[i]);  */
					confronto[n]=0;
				}	
			}	
		}	
	}

	for(n=0;n<N;n++){
		if(confronto[n] != 1){
			kill(pid[n],SIGKILL);
		}else{
			token = 't';
			write(pipePadreFiglio[n][1],&token,1);
		}
	}



	
	/* Il padre aspetta i figli */
	for (int i=0; i < N; i++)
	{
		if ((pidFiglio = wait(&status)) < 0)
        	{
                	printf("Errore in wait\n");
                	exit(10);
        	}

		if ((status & 0xFF) != 0)
              		printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{	ritorno=(int)((status >> 8) & 0xFF);
                	if (ritorno == 255) 
				printf("Il figlio con pid=%d ha ritornato il valore %d e quindi ci sono stati problemi\n", pidFiglio, ritorno);
			else
			/* se un figlio termina normalmente vuol dire che non e' stato ucciso dal SIGKILL: ATTENZIONE CHE DOBBIAMO RECUPERARE L'INDICE DI CREAZIONE USANDO L'ARRAY DI pid! */
                		for (int j=0; j < N; j++)
					if (pid[j] == pidFiglio) 
						printf("Il figlio con pid=%d e indice %d ha ritornato il valore %d e quindi ha verificato che il file %s e' uguale al file %s\n", pidFiglio, j, ritorno, argv[j+1], argv[argc-1]);
		}
	}


	exit(0);
}