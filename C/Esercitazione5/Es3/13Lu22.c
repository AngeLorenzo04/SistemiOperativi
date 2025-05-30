/* Soluzione della parte C del compito del 13 Luglio 2022 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

/* definizione del TIPO pipe_t come array di 2 interi */
typedef int pipe_t[2];

int main (int argc, char **argv)
{
   int Q; 				/* numero di file/processi */
   /* ATTENZIONE NOME Q imposto dal testo! */
   int pid;				/* pid per fork */
   pipe_t *pipes;			/* array di Q+1 pipe usate a ring da primo figlio, a secondo figlio .... ultimo figlio e poi padre e quindi di nuovo da primo figlio: ogni processo legge dalla pipe q e scrive sulla pipe q+1; il padre legge da pipe Q e scrive su pipe 0! */
   int q,j; 				/* indici */
   /* ATTENZIONE NOME q imposto dal testo! */
   int L;				/* per valore numero linee del file */
   /* ATTENZIONE NOME L imposto dal testo! */
   int fd; 				/* file descriptor */
   char ch;        			/* carattere letto dai figli dall'unico file */
   int nrChar;				/* contatore occorrenze del carattere cercato per ogni linea */
   char ok;	      			/* carattere letto dai figli dalla pipe precedente e scritto su quella successiva: N.B. valore non importante! */
   int nr,nw;              		/* variabili per salvare i valori di ritorno di read/write da/su pipe */
   int pidFiglio, status, ritorno;	/* per valore di ritorno figli */

	/* controllo sul numero di parametri: 1 file, una sua lunghezza in linea e almeno 2 caratteri */
	if (argc < 5)
	{
		printf("Errore: Necessari per %s almeno 4 parametri (un file, la sua lunghezza in linea e almeno 2 caratteri e invece argc = %d\n", argv[0], argc);
		exit(1);
	}

	/* ricaviamo il numero di linee del file e controlliamo che sia strettamente maggiore di 0 */
	L = atoi(argv[2]);
	if (L <= 0)
	{
		printf("Errore %s non numero intero strettamente positivo\n", argv[2]);
		exit(2);
	}
	
	/* controlliamo che i parametri a partire dal terzo siano singoli parametri */
	for (j=3; j<argc; j++)
		if (strlen(argv[j]) !=1)
		{
			printf("Il parametro %s NON e' un singolo carattere\n", argv[j] );
			exit(3);
		}

	Q = argc-3;	/* calcoliamo il numero di caratteri e quindi di processi da creare */
	printf("DEBUG-Numero di processi da creare %d\n", Q);

	/* il padre imposta di IGNORARE il segnale SIGPIPE */
	signal(SIGPIPE, SIG_IGN);

	/* allocazione dell'array di Q+1 pipe: ATTENZIONE VANNO CREATE Q+1 pipe! */
	if ((pipes=(pipe_t *)malloc((Q+1)*sizeof(pipe_t))) == NULL)
	{
		printf("Errore allocazione pipe\n");
		exit(4); 
	}

	/* creazione delle Q+1 pipe usate a RING con anche il padre */
	for (q=0; q<Q+1; q++)
		if(pipe(pipes[q])<0)
		{
			printf("Errore creazione pipe di indice q = %d\n", q);
			exit(5);
		}

	/* ciclo di creazione dei figli */
	for (q=0; q<Q; q++)
	{
		if ((pid=fork())<0)
		{
			printf("Errore creazione figlio %d-esimo\n", q);
			exit(6);
		}
	
		if (pid == 0)
		{ 	/* codice  del figlio Pq */
			printf("DEBUG-Sono il figlio %d e sono associato al file %s e al carattere %c\n", getpid(), argv[1], argv[q+3][0]);
			/* tutti i figli sono associati all'unico file */
			/* nel caso di errore in un figlio decidiamo di ritornare il valore -1 che sara' interpretato dal padre come 255 (valore NON ammissibile) */

			/* chiusura  dei lati delle pipe non usate nella sincronizzazione */
			/* per capire le chiusure, fare riferimento al commento riportato per l'array pipes */
			for (j=0;j<Q+1;j++)
			{	
				if (j!=q)
					close (pipes[j][0]);
				if (j != (q+1))
					close (pipes[j][1]);
			}
	
			/* apriamo il file in lettura; N.B.: tutti i figli aprono lo stesso file perche' devono avere l'I/O pointer separato! */
			if ((fd=open(argv[1],O_RDONLY))<0)
			{
				printf("Errore: Impossibile aprire il file %s\n", argv[1]);
				exit(-1);
			}
	
			/* inizializziamo il contatore occorrenze del carattere cercato per ogni singola linea */
			nrChar= 0;
			/* con un ciclo leggiamo tutte le linee, come richiede la specifica */
       	 		while (read(fd,&ch,1) != 0)
        		{
          			if (ch == '\n') /* siamo a fine linea */
                		{
					/* dobbiamo aspettare l'ok dal figlio precedente per scrivere */
                			nr=read(pipes[q][0],&ok,sizeof(char));
        				/* per sicurezza controlliamo il risultato della lettura da pipe */
                			if (nr != sizeof(char))
                			{
                       				printf("Errore: Figlio %d ha letto un numero di byte sbagliati %d\n", q, nr);
                       				exit(-1);
                			}

					/* a questo punto si deve riportare su standard output l'indice e il pid del processo corrente, il numero di occorrenze del proprio carattere associato presenti nella linea corrente e il carattere associato */
                			printf("Figlio con indice %d e pid %d ha letto %d caratteri %c nella linea corrente\n", q, getpid(), nrChar, argv[q+3][0]);
					/* ora si deve mandare l'OK in avanti: nota che il valore della variabile ok non ha importanza */
        				nw=write(pipes[q+1][1],&ok,sizeof(char));
        				/* anche in questo caso controlliamo il risultato della scrittura */
        				if (nw != sizeof(char))
        				{
               					printf("Errore: Figlio %d ha scritto un numero di byte sbagliati %d\n", q, nw);
               					exit(-1);
        				}
					/* si deve azzerare il conteggio delle occorrenze, dopo averlo salvato per poterlo tornare correttamente, nel caso la linea corrente sia l'ultima! */
					ritorno = nrChar;
					nrChar = 0;	/* riazzieriamo il contatore per la prossima linea */
        			}	
				else
                       		{
					/* se non siamo a fine linea dobbiamo fare il controllo sul carattere corrente */
          				if (ch == argv[q+3][0]) /* se abbiamo letto il carattere da cercare incrementiamo il contatore delle occorrenze */
						nrChar++;
        			}			
        		}		

			/* ogni figlio deve tornare il numero di occorrenze del proprio carattere trovate nell'ultima linea */
			exit(ritorno);
		}
	} /* fine for */

	/* codice del padre */
	/* chiusura di tutte le pipe che non usa, a parte la prima e l'ultima */
	for (q=0; q < Q+1; q++) 
	{
		if (q != Q)	close (pipes[q][0]);
		if (q != 0) 	close (pipes[q][1]); 
	}
	/* N.B. Poiche' in questo caso il padre e' nel ring, non ci sono problemi di dover lasciare aperti lati di pipe che il padre non usa! */

	for (j=0; j < L; j++)	/* per ogni linea del file */
       	{
		/* il padre deve riportare il numero di linea correntemente analizzata dai figli, insieme con il nome del file */
		printf("Linea %d del file %s\n", j+1, argv[1]);	/* il numero di linea deve partire da 1! */
		/* il padre deve inviare l'OK di sincronizzazione al processo di indice 0 */
		nw=write(pipes[0][1],&ok,sizeof(char));
		/* anche in questo caso controlliamo il risultato della scrittura */
		if (nw != sizeof(char))
		{
			printf("Errore: Padre ha scritto un numero di byte sbagliati %d\n", nw);
                       	/* andiamo comunque ad aspettare i figli */
		}

		/* il padre quindi deve aspettare che l'ultimo figlio gli invii l'OK di sincronizzazione per fare ripartire il ring */
		nr=read(pipes[Q][0],&ok,sizeof(char));
        	/* per sicurezza controlliamo il risultato della lettura da pipe */
                if (nr != sizeof(char))
                {
                	printf("Errore: Padre ha letto un numero di byte sbagliati %d\n", nr);
                       	/* andiamo comunque ad aspettare i figli */
                }
        }

	/* Il padre aspetta i figli */
	for (q=0; q < Q; q++)
	{
        	if ((pidFiglio = wait(&status)) < 0)
        	{
                	printf("Errore in wait\n");
                	exit(8);
        	}
        	if ((status & 0xFF) != 0)
                	printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
        	else
        	{	 
			ritorno=(int)((status >> 8) & 0xFF);
        		printf("Il figlio con pid=%d ha ritornato %d (se 255 problemi)\n", pidFiglio, ritorno);
        	} 
	}

	exit(0);
}
