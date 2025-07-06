/* Soluzione parte C esame del 10 Luglio 2019 */
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/wait.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 

/* definizione del TIPO pipe_t come array di 2 interi */
typedef int pipe_t[2]; 		

int main(int argc, char **argv) 
{
   int N; 		/* numero di file: i processi figli saranno il doppio! */
   int pid;		/* variabile per fork */
   pipe_t *pipe_f;	/* array di pipe per la comunicazione dai primi N figli agli ultimi N figli */
   pipe_t *pipe_fbis;	/* array di pipe per la comunicazione dagli ultimi N figli ai primi N figli */
   /* OSSERVAZIONE: creare due array separati rende piu' facile il calcolo dell'indice giusto da usare */
   int fd;		/* variabile per open */
   char ch;		/* variabile per leggere dai figli */
   char Cz;		/* variabile per tenere traccia del carattere da cercare */
   int occ;		/* variabile per tenere traccia del numero di occorrenze trovate */
   long int pos;	/* posizione corrente del carattere trovato: inviamo il valore ricavato dalla lseek decrementato di 1 dato che dopo la lettura l'I/O pointer e' posizionato sul carattere seguente quello letto: quindi si considera che il primo carattere di ogni file sia in posizione OL! */
   long int posLetta;	/* posizione corrente del carattere trovato ricevuta */
   int status, pidFiglio, ritorno;	/* per wait */
   int i, j;		/* indici per cicli */
   int nr, nw;		/* per controlli read e write su/da pipe */

/* Controllo sul numero di parametri: almeno 2 nomi di file e carattere */
if (argc < 4) 
{	
	printf("Errore: Necessari per %s almeno 3 parametri, almeno 2 nomi di file e carattere e invece argc = %d\n", argv[0], argc);
	exit(1);
}

/* calcoliamo il numero dei file: ATTENZIONE BISOGNA TOGLIERE 2 PERCHE' C'E' ANCHE IL CARATTERE Cz */
N = argc - 2; 

/* controlliamo se l'ultimo parametro e' un singolo carattere */
if (strlen(argv[argc-1]) != 1)
{
        printf("Errore ultimo parametro non singolo carattere %s\n", argv[argc-1]);
        exit(2);
}

/* isoliamo il carattere che devono cercare i figli */
Cz = argv[argc-1][0]; 

printf("DEBUG-Sono il padre con pid %d e creo %d figli che dovranno cercare il carattere '%c'\n", getpid(), N, Cz);

/* allocazione memoria dinamica per pipe_f e pipe_fbis */
pipe_f=malloc(N*sizeof(pipe_t));
pipe_fbis=malloc(N*sizeof(pipe_t));
if ((pipe_f == NULL) || (pipe_fbis == NULL))
{
	printf("Errore nelle malloc\n");
	exit(3);
}

/* creazione delle N + N pipe */
for (i=0; i < N; i++) 
{
	if (pipe(pipe_f[i])!=0) 
	{
		printf("Errore creazione della pipe %d-esima fra primi N figli e gli ultimi N\n", i);
		exit(4);
	}
	if (pipe(pipe_fbis[i])!=0) 
	{
		printf("Errore creazione della pipe %d-esima fra ultimi N figli e i primi N\n", i);
		exit(5);
	}
}

/* ciclo di generazione dei figli: ne devono essere creati 2 * N */
for (i=0; i < 2*N; i++) 
{
	if ( (pid = fork()) < 0)
   	{
		printf("Errore nella fork con indice %d\n", i);
      		exit(6);
   	}

	if (pid == 0) 
	{
 		/* codice del figlio */
      		if (i < N) /* siamo nel codice dei primi N figli */
		{
			printf("DEBUG-Figlio di indice %d e pid %d associato al file %s\n",i,getpid(), argv[i+1]);
      		
			/* chiudiamo le pipe che non servono */
      			/* ogni figlio scrive solo su pipe_f[i] e legge solo da pipe_fbis[i] */
      			for (j=0;j < N;j++)
      			{
        			close(pipe_f[j][0]);
        			close(pipe_fbis[j][1]);
        			if (j!=i)
        			{
        				close(pipe_f[j][1]);
        				close(pipe_fbis[j][0]);
        			}
      			}

			/* per i primi N processi, il file viene individuato come al solito */
			if ((fd=open(argv[i+1], O_RDONLY)) < 0)
			{
				printf("Errore: Impossibile aprire il file %s\n", argv[i+1]);
				exit(0); /* in caso di errore torniamo 0 che non e' un valore accettabile (per quanto risulta dalla specifica della parte shell) */
			}

			/* inizializziamo occ */
			occ=0;

			/* leggiamo un carattere alla volta */
	 		while (read(fd, &ch, 1)) 
			{
				if (ch == Cz) /* se abbiamo trovato il carattere da cercare */
				{
					/* incrementiamo occ */
					occ++;
					/* calcoliamo la posizione del carattere */
					/* il valore ricavato dalla lseek lo decrementiamo di 1 dato che dopo la lettura l'I/O pointer e' posizionato sul carattere seguente quello letto */
					pos=lseek(fd, 0L, SEEK_CUR) - 1L;		
					/* printf("DEBUG- VALORE DI pos %ld per processo di indice %d che sto per mandare su pipe_f[i][1] %d\n", pos, i, pipe_f[i][1]); */
					/* inviamo la posizione del carattere all'altro processo della coppia */
					nw=write(pipe_f[i][1], &pos, sizeof(pos));
					if (nw != sizeof(pos))
    					{		
                        			printf("Errore: Impossibile scrivere sulla pipe per il processo di indice %d\n", i);
                        			exit(0); 
               	 			}
					/* aspettiamo dall'altro processo della coppia la nuova posizione da cui si deve riprendere la ricerca */
					nr=read(pipe_fbis[i][0], &posLetta, sizeof(posLetta));
					if (nr != sizeof(posLetta))
    					{		
                        			/* se non viene inviata alcuna posizione vuole dire che l'altro processo della coppia NON ha trovato altre occorrenze e quindi si puo' terminare la lettura */
						break;
         		 		}		
					/* printf("DEBUG-VALORE DI pos %ld per processo di indice %d che ho ricevuto da pipe_fbis[i][0] %d\n", pos, i, pipe_fbis[i][0]); */
					/* spostiamo l'I/O pointer nella posizione seguente a quella ricevuta! */
					lseek(fd, posLetta+1L, SEEK_SET);
				} 
				/* l'else non serve dato che NON si deve fare nulla e si deve continuare a leggere */
			}
		}
		else /* siamo nel codice degli ultimi N figli */
		{
			printf("DEBUG-SECONDA SERIE DI FIGLI-Figlio di indice %d e pid %d associato al file %s\n",i,getpid(), argv[2*N-i]); /* ATTENZIONE ALL'INDICE CHE DEVE ESSERE USATO */

                 	/* chiudiamo le pipe che non servono */
                 	/* ogni figlio scrive solo su pipe_fbis[i] e legge solo da pipe_f[i] */
                 	for (j=0;j < N; j++)
                 	{
                   		close(pipe_f[j][1]);
                        	close(pipe_fbis[j][0]);
                        	if (j!= 2*N-i-1)	 /* ATTENZIONE ALL'INDICE CHE DEVE ESSERE USATO */
                        	{
                        		close(pipe_f[j][0]);
                                	close(pipe_fbis[j][1]);
                        	}
              		}

			/* per gli ultimi N processi, il file viene individuato come indicato nel testo! */
                	if ((fd=open(argv[2*N-i], O_RDONLY)) < 0)
                	{
                		printf("Errore: Impossibile aprire il file %s\n", argv[2*N-i]);
                        	exit(0); /* in caso di errore torniamo 0 che non e' un valore accettabile (per quanto risulta dalla specifica della parte shell) */
                	}

                	/* inizializziamo occ */
                	occ=0;
                
			/* per prima cosa dobbiamo aspettare la posizione dall'altro figlio */
			nr=read(pipe_f[2*N-i-1][0], &posLetta, sizeof(posLetta));
                	if (nr != sizeof(posLetta))
                	{
                		printf("Errore: Impossibile leggere dalla pipe per il processo di indice %d (PRIMA LETTURA)\n", i);
                        	exit(0);
                	}
			/* printf("DEBUG- VALORE DI pos %ld per processo di indice %d che ho ricevuto da pipe_fbis[2*N-i-1][0] %d\n", pos, i, pipe_fbis[2*N-i-1][0]); */
                	/* spostiamo l'I/O pointer nella posizione seguente a quella ricevuta! */
                	lseek(fd, posLetta+1L, SEEK_SET);
			
			/* quindi, leggiamo un carattere alla volta */
                	while (read(fd, &ch, 1))
                	{
				if (ch == Cz) /* se abbiamo trovato il carattere da cercare */
                        	{
                        		/* incrementiamo occ */
                                	occ++;
                                	/* calcoliamo la posizione del carattere */
                                	/* il valore ricavato dalla lseek lo decrementiamo di 1 dato che dopo la lettura l'I/O pointer e' posizionato sul carattere seguente quello letto */
                                	pos=lseek(fd, 0L, SEEK_CUR) - 1L;
					/* inviamo la posizione del carattere all'altro processo della coppia */
					/* printf("DEBUG- VALORE DI pos %ld per processo di indice %d che sto per mandare su pipe_f[2*N-i-1][1] %d\n", pos, i, pipe_f[2*N-i-1][1]); */
                                	nw=write(pipe_fbis[2*N-i-1][1], &pos, sizeof(pos));
                                	if (nw != sizeof(pos))
                                	{
                                 		printf("Errore: Impossibile scrivere sulla pipe per il processo di indice %d\n", i);
                                        	exit(0);
                                	}
                                	/* aspettiamo dall'altro processo della coppia la nuova posizione da cui si deve riprendere la ricerca */
                                	nr=read(pipe_f[2*N-i-1][0], &posLetta, sizeof(posLetta));
                                	if (nr != sizeof(posLetta))
    					{		
                        			/* se non viene inviato alcuna posizione vuole dire che l'altro processo della coppia NON ha trovato altre occorrenze e quindi si puÃ² terminare la lettura */
						break;
         		 		}		
					/* printf("DEBUG- VALORE DI pos %ld per processo di indice %d che ho ricevuto da pipe_fbis[i][0] %d\n", pos, i, pipe_fbis[i][0]); */
                        		/* spostiamo l'I/O pointer nella posizione seguente a quella ricevuta! */
                                	lseek(fd, posLetta+1L, SEEK_SET);
                                }
                                /* l'else non serve dato che NON si deve fare nulla e si deve continuare a leggere */
                        }
        	}

        	/* sia per i processi primi della coppia che secondi della coppia */
        	exit(occ); /* torniamo il numero di occorrenze trovate (supposto dal testo <= di 255) */
	}
}

/* codice del padre*/
/* chiudiamo tutte le pipe, dato che le usano solo i figli */
for (i=0; i < N; i++)
{
   close(pipe_f[i][0]);
   close(pipe_f[i][1]);
   close(pipe_fbis[i][0]);
   close(pipe_fbis[i][1]);
}

/* Attesa della terminazione dei figli */
for(i=0;i<2*N;i++)
{
   pidFiglio = wait(&status);
   if (pidFiglio < 0)
   {
      printf("Errore wait\n");
      exit(7);
   }
   if ((status & 0xFF) != 0)
                printf("Figlio con pid %d terminato in modo anomalo\n", pidFiglio);
   else
   {
                ritorno=(int)((status >> 8) & 0xFF);
                printf("Il figlio con pid=%d ha ritornato %d (se 0 problemi!)\n", pidFiglio, ritorno);
   }
}

exit(0);
}/* fine del main */