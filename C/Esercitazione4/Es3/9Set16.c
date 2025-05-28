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
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */


typedef struct {
    char v1; /* carattere */
    long int v2; /* occorrenze */
} data_s;


void bubbleSort(data_s *v, int dim)
{
   int i;
   data_s tmp;
   int ordinato = 0;
	while(dim > 0 && !ordinato)
	{
		ordinato = 1;
		for(i=0; i < dim-1; i++)
		{
			if(v[i].v2 > v[i+1].v2)
			{
				tmp = v[i];
				v[i] = v[i+1];
				v[i+1] = tmp;
				ordinato = 0;
			}
		}
		dim--;
	}
}

int main(int argc, char* argv[]) {
    

    /*------ VARIABILI USATE ------*/
    int fd;                     /* file descriptor */
    char Cx;                    /* variabile che conterrà un singolo carattere */
    int pid;                    /* Variabile che conterrà il valore di ritorno di fork */
    int status;                 /* Variabile che conterrà lo stato di un figlio */
    int pidFiglio;              /* Variabile che conterrà il pid del figlio*/
    int ritorno;                /* Variabile che conterrà il ritrono del filgio*/
    pipe_t pipeGlobal[26];
    int n;
    int N=26;
    data_s data[26];

    /*Controllo parametri STRETTO*/
    if (argc != 2){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
        exit(1);
    }

    /* Creazione delle N pipe */
    for(n=0;n<N;n++){
       if(pipe(pipeGlobal[n]) < 0 ){
          printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
          exit(3);
       }
    }    

    for(n=0; n<N;n++){
        if ((pid = fork()) < 0) { /* creazione con verifica del processo figlio */
            /* fork fallita */
            printf("Errore in fork\n");
            exit(6);
        }
        
        if (pid == 0) {
             /* codice eseguito dal processo figlio*/

             /* Chiusura del canale di lettura di N pipe */
             for(int k=0;k<N;k++){
                if(k != (n-1) || n == 0){
                    close(pipeGlobal[k][0]); /* chiusura della lettura */
                }
                if(k != n){
                    close(pipeGlobal[k][1]); /* Chiusura dei canali di scrittura che non ci interessano */
                }
            }

            /* controllo che il parametro sia un file */
            if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
               printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
               exit(1);
            }



            if(n != 0){
                if (read(pipeGlobal[n-1][0],data,sizeof(data)) < 0)
                {
                    printf("ERRORE: qualcosa è andato storto nella lettura\n");
                    exit(-1);

                }

            }

            /* inizializzo l'n-esima struct */
            data[n].v1 = 'a' + n;
            data[n].v2 = 0L;
            /* cerco le occorrenze del carattere corrente nel file*/
            while (read(fd,&Cx,1) == 1) {
                if(Cx == ('a' + n)){
                    data[n].v2++; /* aggiorno i valori della struct */
                }
            }
            

            if(write(pipeGlobal[n][1],data, sizeof(data)) < 0){
                printf("ERRORE: qualcosa e' andato storto nellla scrittura\n");
                exit(-1);
            }        

            exit(Cx);
        }
    }

   /* Chiusura del canale di scrittura di N pipe */
   for(n=0;n<N;n++){
      close(pipeGlobal[n][1]);
      if(n != N - 1){
        close(pipeGlobal[n][0]);
      }
   }

   if(read(pipeGlobal[N-1][0],data,sizeof(data)) < 0){
        printf("ERRORE: qualcosa e' andato storto nella lettura \n");
   }

   bubbleSort(data,N); /* ordino */

    for(n=0;n<N;n++){
        printf("DEBUG-PADRE\nn:%d\nV1:%c\nV2:%ld\n",n+1,data[n].v1,data[n].v2);
    }

   for(n=0;n<N;n++){
    if((pidFiglio = wait(&status)) < 0){ // assegamo a pid il pid del figlio e a status lo stato del filgio 
        printf("ERRORE: wait ha fallito\n");
        exit(7);
    }
    if ((status & 0xFF) != 0){ //verificare che lo status sia valido2
       printf("Figlio terminato in modo anomalo\n");
        exit(8);
    }else { //estraggo il valore passato dal figlio
        ritorno = status >> 8;
        /* selezione degli 8 bit piu’ significativi */
        ritorno &= 0xFF;
        printf("DEBUG-PADRE il filgio %d ha ritornato %d\n",pidFiglio,ritorno);
    }
   }

    exit(0);
}