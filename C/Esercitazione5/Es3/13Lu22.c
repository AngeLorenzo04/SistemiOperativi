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
#include <signal.h>
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */

int main(int argc, char* argv[]) {
	
	/*------ VARIABILI USATE ------*/
	int Q; 					   /* Varia che contiene il numero di caratteri */
	int F; 				   	   /* Variabile che contiene il filedescriptor del file  */
	int L;					   /* Variabile che contiene la lunghezza in linee del file */
	int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
	int status;                /* Variabile che conterrà lo stato di un figlio */
	int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
	int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
	int n;                 	   /* Contatore */
	int nChar;
	char token;
	char Cx;
	pipe_t* pipeRing;   /* Canale di comunicazione padre-filgio */



	/*Controllo parametri LASCO*/
	if (argc < 4){
		printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3 argomenti\n");
		exit(-1);
	}


	Q=argc-3;
	if((L=atoi(argv[argc - Q - 1]) ) < 0){
		printf("ERRORE: il parametro %s non valido\nSUGGERIMENTO: inserire un valore numerico\n",argv[argc - Q - 1]);
		exit(-1);
	}

	/* allocazione dell'array di N pipe */
	pipeRing = (pipe_t *)malloc((Q  + 1) * sizeof(pipe_t));
	if (pipeRing == NULL) {
	   printf("Errore nella allocazione array dinamico per le pipe\n");
	   exit(3);
	}
	/* Creazione delle N pipe */
	for(n=0;n<Q + 1;n++){
	   if(pipe(pipeRing[n]) < 0 ){
		  printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
		  exit(3);
	   }
	}

	signal(SIGPIPE, SIG_IGN);
 
	for(n=0;n<Q;n++){
		if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
			/* fork fallita */
			printf("Errore in fork\n");
			exit(-1);
		}
		if (pid == 0) {
		 /* codice eseguito dal processo figlio*/

			/* Chiusura del canale di lettura di N pipe */
			for (int j=0;j<Q+1;j++)
			{	
				if (j!=n)
					close (pipeRing[j][0]);
				if (j != (n+1))
					close (pipeRing[j][1]);
			}

			/* controllo che il parametro sia un file */
			if((F=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
			   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
			   exit(-1);
			}

			nChar=0;
       	 	while (read(F,&Cx,1) == 1){
				if(Cx == '\n'){
					int nr=read(pipeRing[n][0],&token,sizeof(char));
        			/* per sicurezza controlliamo il risultato della lettura da pipe */
                	if (nr != sizeof(char))
                	{
                    		printf("Errore: Figlio %d ha letto un numero di byte sbagliati %d\n", n, nr);
                    		exit(-1);
                	}                	printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", n, getpid(), nChar, argv[n+3][0]);
					write(pipeRing[n+1][1],&token,sizeof(char));
					ritorno=nChar;
					nChar=0;
				}else{
					if(Cx == argv[n+3][0]){
						nChar++;
					}
				}
				

			}
					   
		   exit(ritorno);
		}
	}

	/* Chiudo tutte le pipe inutili */
	for(n=0;n<Q + 1;n++){

		if(n != 0){
			close(pipeRing[n][1]);
		}
		if(n != Q){
			close(pipeRing[n][0]);
		}

	}



	for (int j=0; j < L; j++)	/* per ogni linea del file */
       	{
		/* il padre deve riportare il numero di linea correntemente analizzata dai figli, insieme con il nome del file */
		printf("Linea %d del file %s\n", j+1, argv[1]);	/* il numero di linea deve partire da 1! */
		/* il padre deve inviare l'OK di sincronizzazione al processo di indice 0 */
		int nw=write(pipeRing[0][1],&token,sizeof(char));
		/* anche in questo caso controlliamo il risultato della scrittura */
		if (nw != sizeof(char))
		{
			printf("Errore: Padre ha scritto un numero di byte sbagliati %d\n", nw);
                       	/* andiamo comunque ad aspettare i figli */
		}

		/* il padre quindi deve aspettare che l'ultimo figlio gli invii l'OK di sincronizzazione per fare ripartire il ring */
		int nr=read(pipeRing[Q][0],&token,sizeof(char));
        	/* per sicurezza controlliamo il risultato della lettura da pipe */
                if (nr != sizeof(char))
                {
                	printf("Errore: Padre ha letto un numero di byte sbagliati %d\n", nr);
                       	/* andiamo comunque ad aspettare i figli */
                }
        }

	for(n=0;n<Q;n++){


		if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
			printf("ERRORE: wait ha fallito\n");
			exit(-1);
		}
		if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
		   printf("Figlio terminato in modo anomalo\n");
			exit(-2);
		}else { //estraggo il valore passato dal figlio
			ritorno = status >> 8;
			/* selezione degli 8 bit piu’ significativi */
			ritorno &= 0xFF;
			printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
			
		}	


	}	




	exit(0);
}