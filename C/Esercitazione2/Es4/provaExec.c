/* Programma in C provaExec.c che non ha bisogno di parametri e che esegue ricorsivamente lo stesso programma eseguibile sulla base del valore intero fornito dall'utente, 0 per terminare; si e' deciso di usare la primitiva execv */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
   char *av[2]; 	/* array statico di puntatori a char che serve per passare i parametri alla execv: bastano due elementi, uno servira' per il nome del corrispondente eseguibile (invocato ricorsivamente) e il secondo per il terminatore della lista di stringhe */
   int x;		/* per leggere un valore dallo standard input */

	av[0]="./provaExec";	/* av[0] e' un puntatore a char cui viene assegnato il puntatore alla stringa "provaExec" */
	av[1]= (char *)0;	/* av[1] e' un puntatore a char cui viene assegnato il valore 0 come puntatore a char (cioe' il terminatore della lista di stringhe!) */

	printf("Esecuzione di %s\n", av[0]);
	printf("Dimmi se vuoi finire (valore 0)!\n");
	scanf("%d", &x);
	if (x != 0)	/* se il valore e' diverso da 0 allo si invoca ricorsivamente lo stesso comando */
	{
		printf("DEBUG-Sto per eseguire %s\n", av[0]);
		execv(av[0], av);

		/* si esegue l'istruzione seguente SOLO in caso di fallimento della execv */
		printf("Errore in execv\n");
		exit(1);	/* torniamo quindi un valore diverso da 0 per indicare che ci sono stati dei problemi */
	}
	
	exit(x);	/* notare che x sara' 0! */
}