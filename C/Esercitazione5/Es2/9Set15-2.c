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
#include <signal.h> //ISoType: isdigit, isalpha, isalnum
#define PERM 077 /*Permessi da dare al file creato*/
typedef int pipe_t[2]; /* Tipo per creare più pipe */


void handler(int signo) {
    printf("DEBUG-Sono il padre %d e ho ricevuto il segnale %d\n", getpid(), signo);
}

int main(int argc, char const *argv[]) {

    signal(SIGPIPE, handler);

    int AF;                    /* file descriptor */
	int Fn;					   /* File descriptor */
	char Cx;                   /* variabile che conterrà un singolo carattere del filgio */
	char c;					   /* Variabile che conterrà un singolo carattere del padre */
	int* pid;                  /* Variabile che conterrà il valore di ritorno di fork */
	int status;                /* Variabile che conterrà lo stato di un figlio */
	int pidFiglio;             /* Variabile che conterrà il pid del figlio*/
	int ritorno;               /* Variabile che conterrà il ritrono del filgio*/
	int n;					   /* Variabile per scorrere */
	int N;                	   /* Numero di figli */
	int* confronto;
	char token='v';
	pipe_t* pipePadreFiglio;   /* Canale di comunicazione padre-filgio */
	pipe_t* pipeFiglioPadre;
    
    if (argc < 3) {
		printf("ERRORE: numero parametri insufficiente\nSUGGERIMENTO: inserire almeno 2 argomenti\n");
		exit(1);
	}

    N = argc - 2;

    

    if ((pid = (int *) malloc(N * sizeof(int))) == NULL) {
        printf("Errore nell'allocazione dell'array dei pid\n");
        exit(2);
    }

    if ((confronto = (int *) malloc(N * sizeof(int))) == NULL) {
        printf("Errore nell'allocazione dell'array dei confronti\n");
        exit(3);
    }

    for (n = 0; n < N; n++) {
        confronto[n] = 1;
    }

    if ((pipeFiglioPadre = (pipe_t *) malloc(N * sizeof(pipe_t))) == NULL) {
        printf("Errore nell'allocazione delle pipe per la comunicazione figlio-padre\n");
        exit(4);
    }

    if ((pipePadreFiglio = (pipe_t *) malloc(N * sizeof(pipe_t))) == NULL) {
        printf("Errore nell'allocazione pipe per la comunicazione padre-figlio (per sincronizzazione con token 'v')\n");
        exit(5);
    }

    for(n = 0; n < N; n++) {
        if (pipe(pipeFiglioPadre[n]) < 0) {
            printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
            exit(6);
        }

        if (pipe(pipePadreFiglio[n]) < 0) {
            printf("ERRORE: qualcosa e' andato storto nella creazione delle pipe\n");
            exit(7);
        }
    }

    for (n = 0; n < N; n++) {
        if ((pid[n] = fork()) < 0) {
            printf("Errore nella creazione del processo figlio di indice %d\n", n);
            exit(8);
        }

        if (pid[n] == 0) {
            for (int i = 0; i < N; i++) {
                close(pipeFiglioPadre[i][0]);
                close(pipePadreFiglio[i][1]);
                if (i != n) {
                    close(pipeFiglioPadre[i][1]);
                    close(pipePadreFiglio[i][0]);
                }
            }

            if ((Fn = open(argv[n+1], O_RDONLY)) < 0) {
                printf("Errore: non è stato possibile aprire il file %s\n", argv[n+1]);
                exit(-1);
            }

            while (1) {
                int nread = read(pipePadreFiglio[n][0], &token, 1);
                printf("nread = %d, token = %c\n", nread, token);
                if (token == 't') {
                    break;
                }

                read(Fn, &Cx, 1);
                printf("[FIGLIO %d] %c %s\n", n, Cx, argv[n+1]);
                write(pipeFiglioPadre[n][1], &Cx, 1);
            }

            exit(0);
        }
    }

    for (n = 0; n < N; n++) {
        close(pipePadreFiglio[n][0]);
        close(pipeFiglioPadre[n][1]);
    }

    if ((AF = open(argv[argc - 1], O_RDONLY)) < 0) {
        printf("Errore: non è stato possibile aprire il file %s\n", argv[argc - 1]);
        exit(9);
    }

    while (read(AF, &c, 1)) {
        for (n = 0; n < N; n++) {
            if (confronto[n]) {
                write(pipePadreFiglio[n][1], &token, 1);

                read(pipeFiglioPadre[n][0], &Cx, 1);
                if (c != Cx) {
                    confronto[n] = 0;
                }
            }
        }
    }

    for (n = 0; n < N; n++) {
        if (!confronto[n]) {
            if (kill(pid[n], SIGKILL) == -1) {
                printf("Figlio con pid %d è già terminato, non è stato possibile mandare il segnale SIGKILL\n", pid[n]);
            } 
        } else {
            token = 't';
            write(pipePadreFiglio[n][1], &token, 1);
        }
    }

    

    for (n = 0; n < N; n++) {
        if ((pidFiglio = wait(&status)) < 0) {
            printf("Errore in wait\n");
            exit(10);
        }

        if ((status & 0xFF) != 0) {
            printf("Figlio con pid %d è terminato in modo anomalo\n", pidFiglio);
        } else {
            ritorno = (int) ((status >> 8) & 0xFF);
            if (ritorno == 255) {
                printf("Il figlio con pid %d ha ritornato il valore %d, cioè è successo qualcosa di anomalo\n", pidFiglio, ritorno);
            } else {
                for (int i = 0; i < N; i++) {
                    if (pid[i] == pidFiglio) {
                        printf(
                            "Il figlio con pid %d e indice %d ha ritornato il valore %d, perciò il file %s è uguale al file %s\n", 
                            pidFiglio, i, ritorno, argv[i+1], argv[argc-1]
                        );
                    }
                }
            }
        }
    }

    exit(0);
}
