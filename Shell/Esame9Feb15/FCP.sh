#!/bin/sh
#FILE COMANDI PRINCIPALE PER LA PROVA D'ESAME DEL 9 FEBBRAIO 2015
#controllo stretto dei parametri 

if test $# -ne 2
then 
	echo ERROR: numero parametri non valido
	exit 1
fi

#Arrivati qui il numero parametri è corretto
#Controlli sul primo parametro

case $1 in 
	/*);;
	*)
	       	echo ERROR: il parametro 1 deve eseere un percorso assoluto
		exit 2;;
esac
#Controlli sul secondo parametro
expr $2 + 0 > /dev/null 2>&1
result=$?
if test $result -eq 0
then
	if test $2 -lt 0
	then 
		echo ERROR: il secondo parametro deve essere un valore numerico positivo
		exit 3
	fi
else
	echo ERROR: il secondo parametro deve essere numerico positivo
fi
#Arrivati qui i parametri sono corretti
#Esporto la variabile PATH
PATH=`pwd`:$PATH
export PATH

FCR.sh $*
