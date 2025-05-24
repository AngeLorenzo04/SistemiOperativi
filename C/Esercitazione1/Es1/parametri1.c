#include <stdio.h>
#include <stdlib.h>
int main (int argc , char*argv[]){

    //controllo parametri LASCO
    if(argc < 2){
        printf("ERRORE: numero parametri insuffieciente \n SUGGERIMENTO: inserire almeno un parametro\n");
        exit(1);
    }

    /* Stampo il nome del programam e il numero di parametri passati */
    printf("::::::::::::::::::::::::::::::::::\n nome del eseguibile: %s\n numero parametri: %d \n::::::::::::::::::::::::::::::::::\n",argv[0],argc - 1);
    int N = argc;
    for (int i = 0; i < N - 1; i++){
        // printf("DEBUG-N %d\n",N);
        // printf("DEBUG-i %d\n",i);
        printf("Parametro %d: %s\n",i + 1,argv[i + 1]);
    }
    printf("::::::::::::::::::::::::::::::::::\n");
    exit(0);
}