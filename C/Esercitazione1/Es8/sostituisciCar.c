/* File sostituisciCar.c cerca un determinato carattere al interno di un file e lo sostituisce con uno spazio vuoto */
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>

int main(int argc, char * argv[]){


    /*-------------- Variabili usate --------------*/
    int fd; /* file descriptor */
    char Cx; /* variabile che conterrà il secondo parametro */
    char buffer; /* Buffer dove verra memorizzato il carattere che stiamo analizzando */

    //Controllo dei parametri stretto
    if(argc != 3){
        printf("ERRORE: numero di parametri non valido\nSUGGERIMENTO: inserire esattamente 2 parametri\n");
        exit(1);
    }

    //Controllo sul primo parametro

    if((fd=open(argv[1],O_RDWR)) < 0 ){ //APRO IL FILE
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire il nome di un file\n",argv[1]);
        exit(2);
    }

    //Controllo sul secondo parametro

    if(strlen(argv[2]) != 1){
        printf("ERRORE: parametro %s non valido\nSUGGERIMENTO: inserire un singolo carattere",argv[2]);
        exit(3);
    }
    Cx=argv[2][0];


    //LEGGO IL FILE un carattere per volta

    while (read(fd, &buffer, 1) == 1) /* ATTENZIONE: dobbiamo leggere un solo carattere alla volta */
	{ 
            printf("DEBUG-ANALYZE %c\n",buffer);
          	if (buffer == Cx)	/* se il carattere corrente e' uguale al carattere passato ... */
                { 	lseek(fd, -1L, 1);	/* SI DEVE RIPORTARE INDIETRO IL FILE POINTER */
			/* si puo' usare anche SEEK_CUR invece che 1 come terzo parametro */
                  	write(fd, "I", 1);	/* si deve scrivere il carattere SPAZIO: N.B. se si vuole si puo' definire una variabile char blank=' ' e quindi passare &blank come secondo parametro */
                }
    }


    exit(0);
}