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
#define PERM 777 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */



int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    char linea[250];           /* Buffer dove verra memorizzato la linea che stiamo analizzando */
    char lineaLetta[250];           /* Buffer dove verra memorizzato la linea che stiamo analizzando */
    char *Fnome;
    int pid;                   /* Variabile che conterrà il valore di ritorno di fork */
    int status;                /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
    int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
    int nread;
    int N;
    int n;
    int fcreato;              /* file descriptor */
    int fd;
    int nro;
    int nriga;
    int i;
    int lenRigaLetta;

    pipe_t* pipeFiglioFiglio;   /* Canale di comunicazione padre-filgio */





    /*Controllo parametri LASCO*/
    if (argc < 2){
        printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 1 argomenti\n");
        exit(1);
    }



    N=argc-1;
    for(n=0;n<N;n++){
        /* controllo che il parametro sia un file */
        if((fd=open(argv[n+1],O_RDWR)) < 0 ){ //APRO IL FILE
           printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n+1]);
           exit(2);
        }
        close(fd);
    }



    /* allocazione dell'array di N pipe */
    pipeFiglioFiglio = (pipe_t *)malloc(N * sizeof(pipe_t));
    if (pipeFiglioFiglio == NULL) {
       printf("Errore nella allocazione array dinamico per le pipe\n");
       exit(3);
    }
    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeFiglioFiglio[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }



    for(n=0;n< 2 * N;n++){
        if(n<N){ /* Creo i figli addetti alle righe dispari */

            if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(4);
            }
            if (pid == 0) {
             /* codice eseguito dal processo figlio*/
                printf("DEBUG-DISPARI creato figlio %d con pid %d\n",n+1,getpid());
                
             /* Chiusura del canale di lettura di N pipe */
             for(int k=0;k<N;k++){
                close(pipeFiglioFiglio[k][0]);
                if(k != n){
                     close(pipeFiglioFiglio[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                }
             }



            fd=open(argv[n+1],O_RDONLY);
            i=0;
            nriga=0;
            nro=0;
            while((nread=read(fd,linea + i,1)) == 1){ /* Leggo il contenuto del file */

                if(linea[i] == '\n' || linea[i] == '\0'){
                    nriga++;
                    if((nriga%2) == 1){
                        i++; /* aggiorno la lunghezz della linea */
                        write(pipeFiglioFiglio[n][1],&i,sizeof(int)); /* invio pirma la lunghezza */
                        write(pipeFiglioFiglio[n][1],linea, sizeof(linea)); /* poi invio la linea */

                    }
                    printf("FIGLIO-DISPARI\nriga:%d\nnro:%d\ni:%d\n",nriga,nro,i);
                    if(nro<i){
                        nro=i;
                    }
                    
                    i=0;
                    
                }else i++;

            }
                printf("DEBUG-DISPARI la riga più lunga ha %d caratteri\n",nro);
            
               exit(nro);
            }
        }else{ /* Creo i figli addetti alle righe pari */

            if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
                /* fork fallita */
                printf("Errore in fork\n");
                exit(4);
            }
            if (pid == 0) {
             /* codice eseguito dal processo figlio*/
                printf("DEBUG-PARI creato figlio %d con pid %d\n",n+1,getpid());

				/* i figli secondi della coppia devono creare il file specificato */
				Fnome=(char *)malloc(strlen(argv[n-N+1]) + 5); /* bisogna allocare una stringa lunga come il nome del file associato + il carattere '.' + i caratteri della parola min (3) + il terminatore di stringa: ATTENZIONE ALL'INDICE PER INDIVIDUARE IL FILE */
				if (Fnome == NULL)
				{
					printf("Errore nella malloc\n");
					exit(-1);
				}
				strcpy(Fnome, argv[n-N+1]);
				strcat(Fnome,".max");
				fcreato=creat(Fnome, PERM);
				if (fcreato < 0)
				{
					printf("Impossibile creare il file %s\n", Fnome);
					exit(-1);
				}
                /* chiudiamo il file creato dato che comunque il figlio non lo usa */
                


                /* Chiusura pipe inutilizzate */
                for(int k=0;k<N;k++){
                 
                    close(pipeFiglioFiglio[k][1]);
                    if(k != n-N){
                        close(pipeFiglioFiglio[k][0]);
                    }

                }


                /* controllo che il parametro sia un file */
                if((fd=open(argv[n-N+1],O_RDWR)) < 0 ){ //APRO IL FILE
                   printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[n-N+1]);
                   exit(-1);
                }
                i=0;
                nriga=0;
                nro=0;
                while (read(fd, &(linea[i]), 1))
				{
					if (linea[i] == '\n') 
			 		{ 
						nriga++; 
						if ((nriga %2) == 0) 
			 			{ 
				   			i++;
							read(pipeFiglioFiglio[n-N][0], &lenRigaLetta, sizeof(lenRigaLetta)); /* leggo la lunghezza della riga mandata dal figlio socio */
							read(pipeFiglioFiglio[n-N][0], &lineaLetta, lenRigaLetta); /* leggo la stringa */
                            printf("DEBUG-PARI messaggi letti:\nlenRigaLetta:%d\nlineaLetta:%s\n",lenRigaLetta,lineaLetta);
							if (i > lenRigaLetta){ /* scrivo la riga piu lunga */
								write(fcreato, linea, i);
							}
							else{
								write(fcreato, lineaLetta, lenRigaLetta);
							}

							/* aggiorno massimo */
							if (i > nro) 
								nro=i; 
						}
				   		i=0; 
					}
					else i++; 
				}
           


                printf("DEBUG-PARI la riga più lunga ha %d caratteri\n",nro);
                exit(nro);
            }
        }
     
    }




    /* Chiusura del canale di scrittura di N pipe */
    for(n=0;n<N;n++){
       close(pipeFiglioFiglio[n][1]);
       close(pipeFiglioFiglio[n][0]);
    }



    for(n=0;n<N * 2;n++){
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
            printf("il figlio %d ha ritornato %d\n",pidFiglio,ritorno);
        }
    }

    exit(0);
}

