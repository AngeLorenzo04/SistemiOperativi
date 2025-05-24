#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char * argv[]){




    /*------------------------------ Variabili locali --------------------------*/
    int N; /* variabile che conterrà il parametro intero positivo */
    char C; /* variabile che conterrà il parametro di tipo char */
    int fd; /* file descriptor per apertura file*/
    /*-------------------------------------------------------------------------*/


    /* controllo parametri stretto */
    if (argc != 4){
        printf("ERRORE: numero parametri non valido \nSUGGERIMENTO: inserire 3 parametri \n");
        exit(1);
    }


    /* controllo sul primo parametro */    
    if ( (fd = open(argv[1],O_RDONLY)) < 0) {
        printf("ERRORE: impossibile aprire il file %s\nSUGGERIMENTO: inserire il nome di un file esistente e legibile\n",argv[1]);
        exit(2);        // printf("DEBUG-N %d\n",N);
        // printf("DEBUG-i %d\n",i);
    }
    /* controllo sul secondo parametro */
    N = atoi(argv[2]);
    if (N <= 0){
        printf("ERRORE: parametro non valido %s\nSUGGERIMETNO: inserire un intero positivo\n",argv[2]);
        exit(3);
    }

    /* controllo sul terzo parametro */
    if (strlen(argv[3]) != 1){
        printf("ERRORE parametro non valido %s\nSUGGERIMENTO: inserire un singolo carattere\n",argv[3]);
        exit(4);
    }
    C = argv[3][0];

    /* Stampo il nome del programam e il numero di parametri passati */
    printf("::::::::::::::::::::::::::::::::::\n nome del eseguibile: %s\n numero parametri: %d \n::::::::::::::::::::::::::::::::::\n",argv[0],argc - 1);
    printf("Parametro %d: %s -> nome di un file\n",1,argv[1]);
    printf("Parametro %d: %d -> intero positivo\n",2,N);
    printf("Parametro %d: %c -> carattere\n",3,C);
    printf("::::::::::::::::::::::::::::::::::\n");

    

    exit(0);

    

}