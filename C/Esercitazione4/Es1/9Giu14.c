#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

typedef int pipe_t[2];

int main(int argc, char * argv[]){


    int fd; /*file descriptor*/
    int n; /*indice del file*/
    int N;/*Numero di file*/
    int pid, pidFiglio, pidNipote, status; /*creazione figli*/
    pipe_t* pipedPadreFiglio;
    pipe_t pipedFiglioNipote;
    int line; /* dove memorizzo le linee dei file*/

/*Controllo numero di parametri*/
    if(argc < 3){
        printf("ERRORE: inserire almeno 2 parametri\n");
        exit(1);
    }

    /*Controllo che siano file */
    N=argc-1;
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
                dup(pipedFiglioNipote[1]);

                /* Chiudo il doppione */
                close(pipedFiglioNipote[1]);

                execlp("wc","wc","-l",argv[n+1]);
                
                
                /* verrà seseguito nel caso la exec da problemi */
                exit(-1);

            }

            close(pipedFiglioNipote[1]);

            if(read(pipedFiglioNipote[0],&line,sizeof(line)) < 0){
                printf("ERRORE: qualcosa e' andato storto nella lettura della pipe filgionipote\n");
                exit(2);
            }            


        }

    }

    exit(0);
}