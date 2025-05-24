/* File mycat1.c che ti permette di aprire tutti i file passati */
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main (int argc, char* argv[]){
    

    /* ----------------------- VARIABILI UTLIZZATE -------------------*/
    int fd; /* file descriptor */
    char buffer[BUFSIZ]; /* buffer dove salvare/reperire i contenuti dei file */
    int N; /* numero di parametri passati */
    int nread=0; /* numero di caratteri letti */

    if(argc == 1){
        exit(1);
    }

    N = argc - 1; //memorizzo il numero di parametri in N

    for(int i = 0; i < N; i++){ //Scorro i parametri
        
        if((fd = open(argv[i + 1], O_RDONLY)) > 0){ // APRO il file corrente
            printf("CONTENUTO %s:\n",argv[i+1]);
            while ((nread=read(fd,buffer,BUFSIZ)) > 0) // LEGGO BUFSIZE caratteri e li metto nel buffer
            {
                write(1,buffer,nread); // STAMPO su STDOUT (1) BUFSIZE caratteri alla volta prendendoli dal buffer
            }
            
        }else{
            printf("ERRORE: il parametro %s non valido\nSUGGERIMENTO: inserire dei file\n",argv[i + 1]);
            exit(1);
        }

        printf("\n");
        printf("\n");
    
    }
    
    
    exit(0);
}