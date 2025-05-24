#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <sys/wait.h> //SYStem WAIT: wait
#include <string.h>


#define PERM 0644

int main(int argc, char* argv[]) {

    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    int pid; /* Variabile che conterrà il valore di ritorno di fork */
    int status; /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio; /* Variabile che conterrà il pid del figlio*/
    int ritorno; /* Variabile che conterrà il ritrono del filgio*/
    int N=argc-1; /* Variabile che contiene il numero di file */
    int n; /* Variabile che serve ad iterari i file */
    char *FOut; /*Variabile che conterrà il nome del file che creiamo */
    int fdw; /* per la creat */

    /*Controllo parametri LASCO*/
    if (argc < 4){
         printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 3\n");
         exit(1);
    }

    for(n=0;n<N;n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(2);
        }
        if (pid == 0) {
         /* codice eseguito dal processo figlio*/
            printf("DEBUG-FIGLIO numero %d pid: %d\n",n + 1,getpid());
            FOut=malloc(strlen(argv[n+1]) + 6); 
            if (FOut == NULL){
                printf("DBUG-FILGIO Errore nelle malloc\n");
                exit(-1);
            }
            strcpy(FOut,argv[n+1]);
            strcat(FOut,".sort");

                         	if ((fdw=creat(FOut, PERM)) < 0)
                	{
                		printf("DEBUG-FILGIO Impossibile creare il file %s\n", FOut);
                        	exit(-1);
                	}
			/* chiudiamo il file creato dato che comunque il figlio non lo usa */
			close(fdw);



            if ((pid = fork()) < 0) { /* creazione con verifica del processo nipote */
                /* fork fallita */
                printf("DEBUG-FIGLIO Errore in fork\n");
                exit(-1);
            }
            if (pid == 0) {
             /* codice eseguito dal processo nipote*/
            
             /*Redirezione dello STDIN per il comando sort*/
                close(0);
                if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
                    printf("DEBUG-NIPOTE ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
                    exit(-1);
                }

               /* chiudiamo lo standard output dato che la specifica indica che il risultato del comando-filtro sort va scritto sul file creato da ogni figlio (padre di questo processo) */
                close(1);
				/* apriamo il file creato in sola scrittura */
				if (open(FOut, O_WRONLY) < 0)
				{	
					printf("Errore: FILE %s NON si riesce ad aprire in scrittura\n", FOut); 
					exit(-1);
				}

                execlp("sort", "sort", (char *)0);
                /*Caso in cui la execlp non va a buon fine*/
                exit(1);
            }
            
            if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
                printf("DEBUG-FIGLIO ERRORE: wait ha fallito\n");
                exit(-1);
            }
            if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
               printf("DEBUG-FILGIO Nipote terminato in modo anomalo\n");
                exit(-1);
            }else { //estraggo il valore passato dal figlio
                ritorno = status >> 8;
                /* selezione degli 8 bit piu’ significativi */
                ritorno &= 0xFF;
                if(ritorno != 1){
                    printf("DEBUG-FILGIO ordinamento del file di indice %d andato a buon fine\n",n + 1);
                }else printf("DEBUG-FIGLIO ordinamento del file di indice %d non andato a buon fine\n",n + 1);
                exit(ritorno);
            }
            
        }
    }
    
    
    
    for(n=0; n<N; n++){
        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("DEBUG-PADRE ERRORE: wait ha fallito %d\n",pidFiglio);
            exit(3);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
            printf("DEBUG-PADRE Figlio terminato in modo anomalo\n");
            exit(4);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
        /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("DEBUG-PADRE ritorno del nipote %d: %d\n",n+1,ritorno);
        }
    }
    


    exit(0);
}