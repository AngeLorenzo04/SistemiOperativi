#include <stdio.h> //Standard I/O: printf, BUFSIZ
#include <fcntl.h> //File CoNTrol: open, O_RDONLY, O_WRONLY, O_RDWR, O_CREAT(crea se non esiste), O_APPEND(solo R o RW)
#include <stdlib.h> //STanDard LIBrary: exit, malloc, calloc, atoi
#include <unistd.h> //UNIx Standard: read, write, close, SEEK_SET, SEEK_CUR, SEEK_END
#define PERM 0644

int main(int argc, char* argv[]) {
    
    /*------ VARIABILI USATE ------*/
    int fd; /* file descriptor */
    char buffer[BUFSIZ]; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */
    int nread; /* Numero variabili lette da read*/

    /*Controllo parametri STRETTO*/
    if (argc != 2){
        printf("ERRORE: numero parametri non valido\nSUGGERIMENTO: inserire esattamente 1 parametri\n");
        exit(1);
    }

    /* uso della open estesa */
    if((fd=open(argv[1], O_WRONLY | O_APPEND | O_CREAT, PERM)) < 0 ){ //APRO/CREO-APRO il file
        printf("ERRORE: qualcosa e' andato storto nella creazione o apertura del file %s\n",argv[1]);
        exit(2);
    }

    while((nread=read(0,&buffer,BUFSIZ)) > 0){ //LEGGO il contenuto dello STDIN e lo scrivo su buffer
       
        write(fd,buffer,nread); //SCRIVO il contenuto del buffer nel file
        
    }


    exit(0);
}