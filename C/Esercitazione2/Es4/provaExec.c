/* Programma in C provaExec.c che non ha bisogno di parametri e che esegue ricorsivamente lo stesso programma eseguibile sulla base del valore intero fornito dall'utente, 0 per terminare; si e' deciso di usare la primitiva execvp dato che l'utente corrente ha la directory "." nella variabile PATH */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main ()
{
   char *argin[2]; 	/* array statico di puntatori a char che serve per passare i parametri alla execvp: bastano due elementi, uno servira' per il nome del corrispondente eseguibile (invocato ricorsivamente) e il secondo per il terminatore della lista di stringhe */
   int x;		/* per leggere un valore dallo standard input */

	argin[0]="./provaExec";	/* argin[0] e' un puntatore a char cui viene assegnato il puntatore alla stringa "provaExec" */
	argin[1]= (char *)0;	/* argin[1] e' un puntatore a char cui viene assegnato il valore 0 come puntatore a char (cioe' il terminatore della lista di stringhe!) */

	printf("Esecuzione di %s\n", argin[0]);
	printf("Dimmi se vuoi finire (valore 0)!\n");
	scanf("%d", &x);
	if (x != 0)	/* se il valore e' diverso da 0 allo si invoca ricorsivamente lo stesso comando */
	{
		printf("DEBUG-Sto per eseguire %s\n", argin[0]);
		execv(argin[0], argin);

		/* si esegue l'istruzione seguente SOLO in caso di fallimento della execvp */
		printf("Errore in execv\n");
		exit(1);	/* torniamo quindi un valore diverso da 0 per indicare che ci sono stati dei problemi */
	}
	
	exit(x);	/* notare che x sara' 0! */
}