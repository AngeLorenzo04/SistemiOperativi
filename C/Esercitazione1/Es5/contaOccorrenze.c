/* File contaOccorrenze.c che conta le occorrenze del carattere passato come secondo parametro nel file il cui nome e' passato come primo parametro*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>



int main(int argc, char * argv[]){


    /*-------------------VARIABILI USATE ------------------*/
    int fd; /* file descriptor per il primo parametro*/
    char Cx; /* variabile per contenere il secondo parametro*/
    char c; /* buffer per la read */
    long int totale=0; /* varaiabile che conterrà il risultato*/




    /* controllo stretto sui parametri */
    if(argc != 3){
        printf("ERRORE: numero di parametri non valido\nSUGGERIMENTO: inserire esattamente 2 parametri\n");
        exit(1);
    }

    /* controllo sul primo parametro */
    
    if((fd=open(argv[1],'r')) < 0){
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file esistente\n",argv[1]);
        exit(2);
    }

    /* controllo sul secondo parametro */
    if(strlen(argv[2]) != 1){
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire esattametne un carattere\n",argv[2]);
        exit(3);
    }
    Cx=argv[2][0]; /*memorizzo il carrattere al interno della variabile*/

    /* cerco il numero di volte in cui il carattere Cx è presente nel file F */
    while (read(fd,&c,1))
    {
        if (c == Cx){
            totale++; /* se il carattere è satato trovato aggiorna il totale */
        }
    }

    /* stampo il risultato */
    printf("::::::::::::::::::::::::::::::::::::::::::\n");
    printf("programma eseguito: %s\nparametri passati: %d\n",argv[0],argc-1);
    printf("::::::::::::::::::::::::::::::::::::::::::\n");

    printf("\n");
    printf("::::::::::::::::::::::::::::::::::::::::::\n");
    printf("occorrenze di %c in %s: %ld\n",Cx,argv[1],totale);
    printf("::::::::::::::::::::::::::::::::::::::::::\n");


    exit(0);

}