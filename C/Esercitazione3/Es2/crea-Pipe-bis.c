/* Programma in C creaPipe-bis.c che ha bisogno di due nomi di file leggibili e verifichera' il comportamento delle primitive open, close e pipe */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main (int argc, char** argv)
{  	
   int fd1, fd2; 	/* variabili per valore di ritorno open */
   int piped[2]; 	/* array di due interi per la pipe */

	if (argc != 3) 		/* per prima cosa (come negli script) controlliamo il numero di parametri ==> ce ne deve essere esattamente 2 e quindi dato che argc conta anche il comando stesso (arvg[0]) controlliamo se argc e' diverso da 3! */
	{ 	printf("Errore nel numero di parametri: ho bisogno di 2 parametri, ma argc = %d\n", argc);
		exit(1); 	/* in caso di errore (esattamente come negli script) dobbiamo uscire tornando sempre un numero diverso! */
	}

	/* se si arriva qui, sono stati passati esattamente 2 parametri, che supponiamo siano nomi di file esistenti e leggibili! */
	/* tentiamo l'apertura in lettura del file il cui nome e' stato passato come primo parametro */
	if ((fd1 = open(argv[1], O_RDONLY)) < 0)  	/* dobbiamo sempre controllare il valore di ritorno delle open: ATTENZIONE ALLE PARENTESI! */
	/* N.B. Unix controllera' i diritti del file rispetto al processo in esecuzione controllando UID e GID effettivi (che si trovano nel descrittore di processo) nei confronti di UID e GID del file (che si trovano nell'Inode)! */
        /* Questa operazione servira' nei compiti per verificare esistenza e leggibilita' dei file che dovranno essere usati dai processi che risolveranno un qualche problema */
	{ 	printf("Errore in apertura file per %s dato che fd1 = %d\n", argv[1], fd1);
        	exit(2); 
	}	
	/* NOTA BENE: non serve l'else perche' in caso di errore si fa la exit! */
	/* se arriviamo qui siamo riusciti ad aprire il primo file perche' esisteva e il processo corrente aveva il diritto di lettura, quindi stampiamo il valore tornato dalla open */
	printf("Valore di fd1 = %d\n", fd1);

	/* tentiamo l'apertura in lettura del file il cui nome e' stato passato come secondo parametro */
	if ((fd2 = open(argv[2], O_RDONLY)) < 0)	/*  apriamo un altro file per verificare che verra' usato un altro elemento della tabella dei file aperti del singolo processo! */
	{ 	printf("Errore in apertura file per %s dato che fd2 = %d\n", argv[2], fd2);
        	exit(3); 
	}
	/* NOTA BENE: non serve l'else perche' in caso di errore si fa la exit! */
	/* se arriviamo qui siamo riusciti ad aprire anche il secondo file perche' esisteva e il processo corrente aveva il diritto di lettura, quindi stampiamo il valore tornato dalla open */
	printf("Valore di fd2 = %d\n", fd2);

	close(fd1);	/* chiudiamo e quindi eliminiamo le informazioni relative all'elemento della tabella dei file aperti identificato da fd1 */

	/* si crea una pipe: si DEVE sempre controllare che la creazione abbia successo! */
	if (pipe (piped) < 0 ) 
	{ 
                printf("Errore nella creazione pipe\n");
		exit (3); 
	}
	printf("Creata pipe con lato di lettura piped[0] = %d \n", piped[0]);
	printf("Creata pipe con lato di scrittura piped[1] = %d \n", piped[1]);

	exit (0);
}