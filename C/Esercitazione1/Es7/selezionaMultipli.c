/* File selezionaMultipli.c che del primo parametro mostra i caratteri multipli del secondo parametro */
#include<stdlib.h>
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc, char* argv[]){


    /*----------------- VARIABILI UTILIZZATE ------------------*/
    int fd; /* file descriptor */
    char buffer[BUFSIZ]; /* buffer */
    int n; /* secondo parametro di tipo int*/
    int i; /* indice del multiplo */
    int nread; /* numero caratteri letti */


    // controllo parametri stretto
    if (argc != 3){
        printf("ERRORE: numero di parametri non valido\nSUGGERIMENTO: inserire esattamente due parametri\n");
        exit(1);
    }

    //controllo primo parametro
    if((fd=open(argv[1], O_RDONLY)) <= 0){
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire un file esistente\n",argv[1]);
        exit(2);
    }
    //controllo secondo parametro
    if((n=atoi(argv[2])) <= 0){
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire un valore numerico\n",argv[2]);
        exit(3);
    }

    /*cerco e stampo tutti i caratteri multipli del secondo parametro*/
    
    i=0;
    while ((nread=read(fd,buffer,n)) > 0 ){
        if(nread == n){
            /* possiamo selezionare l'n-esimo carattere */
			printf("Il carattere multiplo %d-esimo all'interno del file %s e' %c\n", i, argv[1], buffer[n-1]); 
			i++;    /* incrementiamo il conteggio */
        }else printf("ERRORE: file non ha dimensione multipla di %d\n",n);

    }
    

    exit(0);
}