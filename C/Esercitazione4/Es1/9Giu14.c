#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#include <string.h> //STRING: strlen, strcpy, strcat
#include <sys/wait.h> //SYStem WAIT: wait
#define PERM 077 /*Permessi da dare al file creato*/

typedef int pipe_t[2];

int main(int argc, char * argv[]){


    int fd; /*file descriptor*/
    int n; /*indice del file*/
    int N;/*Numero di file*/
    int pid, pidFiglio, pidNipote, status, ritorno; /*creazione figli*/
    pipe_t* pipedPadreFiglio;
    pipe_t pipedFiglioNipote;
    int valore; /* dove memorizzo le linee dei file*/
    char numero[11];
    long int totale; 

/*Controllo numero di parametri*/
    if(argc < 3){
        printf("ERRORE: inserire almeno 2 parametri\n");
        exit(1);
    }

    /*Controllo che siano file */
    N=argc-1;
    printf("DEBUG-PADRE N:%d\n",N);
    for(n=0;n<N;n++){
        if((fd=open(argv[n+1],O_RDONLY))<0){
            printf("il file %s non valido\n",argv[n+1]);
            exit(2);
        }
    }
   
   /* allocazione dell'array di N pipe */
  	pipedPadreFiglio = (pipe_t *)malloc(N * sizeof(pipe_t));
  	if (pipedPadreFiglio == NULL) {
        printf("Errore nella allocazione array dinamico per le pipe\n");
    	exit(3);
  	}
    for(n=0;n<N;n++){
        if(pipe(pipedPadreFiglio[n]) < 0 ){
            printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
            exit(3);
        }
    }

    for(n=0; n<N; n++){
        /* Creo i processi figli*/
        if((pid = fork()) < 0){
            printf("ERRORE: qualcosa e' andato stornot nella creazione de figlioi\n");
            exit(4);
        }
        if(pid == 0){
            printf("DEBUG-FIGLIO %d: %d\n",n+1,getpid());
            for(int k=0;k<N;k++){
                close(pipedPadreFiglio[k][0]);
                if(k != n){
                    close(pipedPadreFiglio[k][1]);
                }
            }

            if(pipe(pipedFiglioNipote) < 0) {
                printf("ERRORE: qualcosa e' andato storto nella creazione della pipeFiglioNipote\n");
                exit(2);
            }

            /*Creazione del nupote */
            if((pid = fork()) < 0){
                printf("ERRORE: qualcosa e' andato stornot nella creazione de figlioi\n");
                exit(3);
            }   
            if(pid==0){

                /*Chiudo le pipe rimasete aperte*/
                close(pipedPadreFiglio[n][1]);
                close(pipedFiglioNipote[0]);

                /*Ridirezione dell STDIN*/
                close(0);
                open(argv[n+1],O_RDONLY);

                /*Ridirezione del STDOUT*/
                close(1);
                dup(pipedFiglioNipote[1]); /* dub duplica un fd nella tabella ci serve a far in modo che la pipe prenda il posto dello STDOUT*/

                /* Chiudo il doppione */
                close(pipedFiglioNipote[1]); 

                execlp("wc","wc","-l",argv[n+1]);
                
                
                /* verrà seseguito nel caso la exec da problemi */
                exit(-1);

            }

            close(pipedFiglioNipote[1]);

            /* adesso il figlio legge dalla pipe un carattere alla volta, fino a che ci sono caratteri inviati dal nipote tramite il comando-filtro wc */
			int j=0;	/* inizializziamo l'indice del buffer numero */
		    while (read(pipedFiglioNipote[0], &(numero[j]), 1))
			{
				j++;	/* incrementiamo l'indice */
			}

			/* all'uscita di questo ciclo while, nell'array numero ci saranno tutti i caratteri numerici corrispondenti al numero di linee del file e come ultimo carattere il terminatore di linea */
			/* trasformiamo l'array di char in stringa sostituendo allo '\n' il terminatore di stringa, controllando pero' di avere letto in effetti qualcosa */
			if (j!=0) /* se il figlio ha letto qualcosa */
			{
				numero[j-1]='\0';
				/* convertiamo la stringa nel numero che bisogna comunicare al padre */
				valore=atoi(numero);
			}
			else 
			{      	/* questo e' il caso che il nipote sia incorso in un errore e che quindi non abbia eseguito il wc -l */
                valore=0;	/* se il figlio non ha letto nulla, inviamo 0 */
            }

            printf("DEBUG-FIGLIO valore: %d\n",valore);
            if(write(pipedPadreFiglio[n][1],&valore,sizeof(valore)) < 0){
                printf("ERRORE: qualcosa e' andato storto nella scrittura\n");
            }

            ritorno=-1;
            if((pidNipote = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
                printf("ERRORE: wait ha fallito\n");
                exit(3);
            }
            if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
               printf("Figlio terminato in modo anomalo\n");
                exit(4);
            }else { //estraggo il valore passato dal figlio
                ritorno = status >> 8;
                /* selezione degli 8 bit piu’ significativi */
                ritorno &= 0xFF;
                printf("DEBUG-FIGLIO il nipote %d ha ritornato %d\n",pidNipote,ritorno);
                ritorno=valore;
            }

            exit(ritorno);

        }

    }

    for(n=0;n<N;n++){
        close(pipedPadreFiglio[n][1]);
    }

    totale=0L;
    for(n=0;n<N;n++){
        if(read(pipedPadreFiglio[n][0], &valore, sizeof(int)) < 0){
            printf("ERRORE: qualcosa e' andato male nella lettura\n");
            exit(5);

        }
        totale+=(long int)valore;
    }



    for(n=0;n<N;n++){
        if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
            printf("ERRORE: wait ha fallito %d\n",n);
            exit(6);
        }
        if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
           printf("Figlio terminato in modo anomalo\n");
            exit(7);
        }else { //estraggo il valore passato dal figlio
            ritorno = status >> 8;
            /* selezione degli 8 bit piu’ significativi */
            ritorno &= 0xFF;
            printf("DEBUG-PADRE il figlilo %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }

    printf("DEBUG-PADRE ho ricevuto %ld righe\n",totale);

    exit(0);
}