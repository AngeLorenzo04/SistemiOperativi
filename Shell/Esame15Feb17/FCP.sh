#!/bin/sh
#FILE COMANDI PRINCIPALE DELLA PROVA D'ESAME DEL 15 FEBBRAIO 2017

#controllo parametri stretto
if test $# -ne 2
then echo 'numero parametri errato'
	exit 1 
fi
# se siamo qui il numero di parametri e' corretto

#controllo che il primo parametro sia il nome assoluto di una directory
case $1 in
	/*) 
		if test ! -d $1 -o ! -x $1 #controllo che il parametro sia una direcotry traversabile
		then
			echo Errore: $1 non e\' una directory o non e\' traversabile
			exit 2
		fi
		;;
			
	*)
	       	echo Errore: $1 il parametro passato non e\' un percorso assoluto
	      	exit 3;; #caso in cui il parametro non è valido
esac
#controllo che il secondo parametro sia un numero strettamente positivo

expr $2 + 0 > /dev/null 2>&1 #se questa somma non da errori allora $2 e' un numero
N1=$? #Variabile che contiene il risultato del operazione precedente
if test $N1 -ne 0
then 
	echo Errore: il secondo parametro non e numerico
	exit 4
else
	if test $2 -lt 0
	then 
		echo Errore: il secondo parametro e negativo
		exit 5
	fi
fi
#Controllo parametri terminato
#Esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH
echo ==========================
FCR.sh $*
